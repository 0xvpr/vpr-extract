/**
 * Created by:      VPR
 * Created:         June 5th, 2022
 *
 * Updated by:      VPR
 * Updated:         June 13th, 2023
 *
 * Description:     A command line tool for the extraction of the .text section
 *                  of 32/64-bit COFF objects, and ELF64 Relocatable objects.
**/

#include "definitions.hpp"

#include <filesystem>
#include <execution>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <cstring>

#ifdef    _WIN32
  #define gnu::hot
  #define gnu::cold
#endif // _WIN32

using io_pair_t = std::pair<std::string, std::string>;
using object_type_t = uint32_t;

constexpr char target_section[6] = ".text";

[[noreturn,gnu::cold]] static inline
void __usage_error(const char* msg, int exit_code, char* const program_name) {
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

[[nodiscard,gnu::hot]] static inline
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

[[gnu::hot]] static inline
bool process_pe_64(std::ifstream& ifs, const io_pair_t& target) {
    ifs.seekg(0, std::ios::beg);

    definitions::IMAGE_FILE_HEADER file_header{};
    ifs.read(reinterpret_cast<char *>(&file_header), sizeof(file_header));

    if (ifs.fail()) [[unlikely]] {
        std::cerr << "Failed to read Image File Header of: " << target.first << ".\n";
        return false;
    }

    ifs.seekg(sizeof(definitions::IMAGE_FILE_HEADER));
    std::vector<definitions::IMAGE_SECTION_HEADER> section_headers(file_header.NumberOfSections);
    ifs.read(reinterpret_cast<char *>(section_headers.data()), file_header.NumberOfSections * sizeof(definitions::IMAGE_SECTION_HEADER));

    if (ifs.fail()) [[unlikely]] {
        std::cerr << "Failed to read Section Header(s) of: " << target.first << ".\n";
        return false;
    }

    definitions::IMAGE_SECTION_HEADER* text_section{nullptr};
    for (const auto& section : section_headers) {
        if (std::strncmp(reinterpret_cast<const char *>(section.Name), target_section, 5) == 0) {
            text_section = const_cast<decltype(text_section)>(&section);
            break;
        }
    }

    if (!text_section) [[unlikely]] {
        std::cerr << "Operation failed: .text section not found." << "\n";
        return false;
    }

    std::vector<char> data(text_section->SizeOfRawData);
    ifs.seekg(text_section->PointerToRawData, std::ios::beg);
    ifs.read(data.data(), text_section->SizeOfRawData);

    if (ifs.fail()) [[unlikely]] {
        std::cerr << "Failed to read text section of: " << target.first << ".\n";
        return false;
    } 

    ifs.close();
    std::stringstream ss;
    ss << "Size of " << target.first << ":\t" << data.size() << "\n";

    std::ofstream outfile(target.second, std::ios::binary);
    outfile.write(data.data(), text_section->SizeOfRawData);

    if (outfile.fail()) [[unlikely]] {
        std::cout << ss.str();
        std::cerr << "Failed to write data to '" << target.second << "'.\n";
        return false;
    } 

    outfile.close();
    std::cout << ss.str() << "Written to '" << target.second << "'.\n" << std::endl;

    return true;
}

[[gnu::hot]] static inline
bool process_elf_64(std::ifstream& ifs, const io_pair_t& target) {
    ifs.seekg(0, std::ios::beg);

    definitions::ElfHeader64 elf_header_64{};
    ifs.read(reinterpret_cast<char *>(&elf_header_64), sizeof(elf_header_64));

    if (ifs.fail()) [[unlikely]] {
        std::cerr << "Failed to read ELF64_Ehdr header of: " << target.first << ".\n";
        return false;
    } 

    definitions::ElfSectionHeader64 section_header{};
    ifs.seekg(static_cast<long>(sizeof(elf_header_64) + elf_header_64.e_shoff), std::ios::beg);
    ifs.read(reinterpret_cast<char *>(&section_header), sizeof(section_header));

    if (ifs.fail()) [[unlikely]] {
        std::cerr << "Failed to read ELF64_Shdr header of: " << target.first << ".\n";
        return false;
    } 

    std::vector<char> data(section_header.sh_size);
    ifs.seekg(static_cast<long>(section_header.sh_offset), std::ios::beg);
    ifs.read((data.data()), static_cast<long>(data.size()));

    if (ifs.fail()) [[unlikely]] {
        std::cerr << "Failed to read text section of: " << target.first << ".\n";
        return false;
    } 

    ifs.close();
    std::stringstream ss;
    ss << "Size of " << target.first << ":\t" << data.size() << "\n";

    std::ofstream outfile(target.second, std::ios::binary);
    outfile.write(data.data(), static_cast<long>(data.size()));

    if (outfile.fail()) [[unlikely]] {
        std::cout << ss.str();
        std::cerr << "Failed to write data to '" << target.second << "'.\n";
        return false;
    } 

    outfile.close();
    std::cout << ss.str() << "Written to '" << target.second << "'.\n" << std::endl;

    return true;
}

[[nodiscard,gnu::hot]]
int process_target(const io_pair_t& target) {
    std::ifstream file{target.first, std::ios::binary};
    if (!file.is_open()) [[unlikely]] {
        std::cerr << "File: " << target.first << " could not be opened.\n";
        return 1;
    }

    std::string filestem = std::filesystem::absolute(target.first).stem().string();
    const_cast<std::string &>(target.second) = filestem + ".bin";

    object_type_t object_type = determine_object_type(file);

    if (object_type) [[likely]] {
        std::cout << "Target:\t" << target.first << " -> " << target.second << "\n";
    } else [[unlikely]] {
        std::cerr << "Target:\t" << target.first << " is not supported.\n";
        return 2;
    }

    switch (object_type) {
        [[likely]]   case object_types::pe32:  {          /* fallthrough */           }
        [[likely]]   case object_types::pe64:  { process_pe_64(file, target);  break; } 
        [[likely]]   case object_types::elf64: { process_elf_64(file, target); break; }
        [[unlikely]] default:                  {                               break; }
    }

    return 0;
};

int main(int argc, char** argv) {
    char* const program_name = argv[0];

    if (argc < 2) {
        __usage_error("No file(s) specified.", -1, program_name);
    }

    --argc;
    ++argv;

    std::vector<io_pair_t> targets(static_cast<unsigned>(argc));
    auto size = targets.size();

    for (size_t i = 0; i < size; ++i) {
        io_pair_t& target = targets.at(i);
        target.first  = argv[i];
    }

    auto result_handler = [](const io_pair_t& target) -> void {
        switch (process_target(target)) {
            [[likely]]   case 0:  { /* fallthrough */ }
            [[unlikely]] case 1:  { /* fallthrough */ }
            [[unlikely]] case 2:  { /* fallthrough */ }
            [[unlikely]] default: {            break; }
        }
    };

    if (targets.size() < std::thread::hardware_concurrency()) [[likely]] {
        std::for_each(std::execution::seq, targets.begin(), targets.end(), result_handler);
    } else [[unlikely]] {
        std::for_each(std::execution::par_unseq, targets.begin(), targets.end(), result_handler);
    }

    return 0;
}
