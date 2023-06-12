/**
 * Created by:      VPR
 * Created:         June 5th, 2022
 *
 * Updated by:      VPR
 * Updated:         June 11th, 2023
 *
 * Description:     A tool for extracting and dumping the .text section
 *                  of a COFF object file.
**/

#include "definitions.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <cstring>

constexpr const char * target_section = ".text";

[[noreturn]] static inline
void __usage_error(const char* msg, int exit_code, const char* program_name) {
    std::cerr << "Usage error: " << msg << "\n"
                 "\n"
                 "example:\n"
                 "  " << program_name << " path/to/file-1.obj path/to/file-2.obj\n"
                 "\n"
                 "  Target: path/to/file-1.obj -> file-1.bin\n"
                 "  Size of path/to/file-1.obj:      420\n"
                 "  Written to 'file-1.bin'.\n"
                 "\n"
                 "  Target: path/to/file-2.obj -> file-2.bin\n"
                 "  Size of path/to/file-2.obj:      69\n"
                 "  Written to 'file-2.bin'.\n";

    exit(exit_code);
}

[[nodiscard]] static inline
uint32_t determine_object_type(std::ifstream& ifs) {
    uint32_t object_type = object_types::unsupported;

    object_type = [&ifs]() -> uint32_t {
        definitions::IMAGE_FILE_HEADER image_file_header{};

        ifs.seekg(0, std::ios::beg);
        ifs.read(reinterpret_cast<char *>(&image_file_header), sizeof(image_file_header));
        if (ifs.fail()) {
            return object_types::unsupported;
        }

        return object_types::pe64 * (image_file_header.Machine == machine_type::x64)
             + object_types::pe32 * (image_file_header.Machine == machine_type::x86);
    }();

    if (object_type) {
        return object_type;
    }

    object_type = [&ifs]() -> uint32_t {
        definitions::ElfHeader64 elf_header_64{};

        ifs.seekg(0, std::ios::beg);
        ifs.read(reinterpret_cast<char *>(&elf_header_64), sizeof(elf_header_64));
        if (ifs.fail()) {
            return object_types::unsupported;
        }

        if ( elf_header_64.e_magic[0] != 0x7F || 
             elf_header_64.e_magic[1] !=  'E' || 
             elf_header_64.e_magic[2] !=  'L' ||
             elf_header_64.e_magic[3] !=  'F' ||
             elf_header_64.e_type     != machine_type::reloc
           )
        {
            return object_types::unsupported;
        }

        if ( elf_header_64.e_magic[4] != object_types::elf64 ) {
            return object_types::elf64;
        }
        
        return object_types::unsupported;
    }();

    if (object_type) {
        return object_type;
    }

    return object_types::unsupported;
}

int main(int argc, char** argv) {
    auto program_name = argv[0];

    if (argc < 2) {
        __usage_error("No file(s) specified.", 1, program_name);
    } 

    --argc;
    ++argv;

    using io_pair_t = std::pair<std::string, std::string>;
    std::vector<io_pair_t> targets(argc & 0xFF);

    // TODO use input filename as prefix
    for (int i = 0; i < argc && i < 0x100; ++i) {
        auto& target = targets.at(i & 0xFF);
        target.first  = argv[i];
    }

    for (const auto& target : targets) {
        std::ifstream file(target.first, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "File: " << target.first << " could not be opened.\n";
            continue;
        }

        auto filestem = std::filesystem::absolute(target.first).stem().string();
        const_cast<std::string &>(target.second) = filestem + ".bin";

        auto object_type = determine_object_type(file);
        if (!object_type) {
            std::cout << "Target:\t" << target.first << " is not supported.\n";
            continue;
        } else {
            std::cout << "Target:\t" << target.first << " -> " << target.second << "\n";
        }

        switch (object_type) {
            [[likely]] case object_types::pe64:
            [[likely]] case object_types::pe32: {
                file.seekg(0, std::ios::beg);

                definitions::IMAGE_FILE_HEADER file_header{};
                file.read(reinterpret_cast<char *>(&file_header), sizeof(file_header));

                file.seekg(sizeof(definitions::IMAGE_FILE_HEADER));
                std::vector<definitions::IMAGE_SECTION_HEADER> section_headers(file_header.NumberOfSections);
                file.read(reinterpret_cast<char *>(section_headers.data()), file_header.NumberOfSections * sizeof(definitions::IMAGE_SECTION_HEADER));

                definitions::IMAGE_SECTION_HEADER* text_section{nullptr};
                for (const auto& section : section_headers) {
                    if (std::strncmp(reinterpret_cast<const char *>(section.Name), target_section, 5) == 0) {
                        text_section = (decltype(text_section))&section;
                        break;
                    }
                }

                if (!text_section) {
                    std::cerr << "Operation failed: .text section not found." << "\n";
                    file.close();
                    return 4;
                }

                std::vector<char> data(text_section->SizeOfRawData);
                file.seekg(text_section->PointerToRawData, std::ios::beg);
                file.read(data.data(), text_section->SizeOfRawData);
                file.close();

                std::cout << "Size of " << target.first << ":\t" << data.size() << "\n";

                std::ofstream outfile(target.second, std::ios::binary);
                outfile.write(data.data(), text_section->SizeOfRawData);
                outfile.close();

                std::cout << "Written to '" << target.second << "'.\n" << std::endl;

                break;
            }
            [[unlikely]] case object_types::elf64: {
                file.seekg(0, std::ios::beg);

                definitions::ElfHeader64 elf_header_64{};
                file.read(reinterpret_cast<char *>(&elf_header_64), sizeof(elf_header_64));

                definitions::ElfSectionHeader64 section_header{};
                file.seekg((long)(sizeof(elf_header_64) + elf_header_64.e_shoff), std::ios::beg);
                file.read(reinterpret_cast<char *>(&section_header), sizeof(section_header));

                std::vector<char> data(section_header.sh_size);
                file.seekg((long)(section_header.sh_offset), std::ios::beg);
                file.read((data.data()), static_cast<long>(data.size()));
                file.close();

                std::cout << "Size of " << target.first << ":\t" << data.size() << "\n";

                std::ofstream outfile(target.second, std::ios::binary);
                outfile.write(data.data(), static_cast<long>(data.size()));
                outfile.close();

                std::cout << "Written to '" << target.second << "'.\n" << std::endl;

                break;
            }
            [[unlikely]] default: { break; }
        }

    }

    return 0;
}
