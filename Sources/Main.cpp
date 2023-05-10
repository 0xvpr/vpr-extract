/**
 * Created by:      VPR
 * Created:         June 5th, 2022
 *
 * Updated by:      VPR
 * Updated:         January 1st, 2023
 *
 * Description:     A tool for extracting and dumping the .text section
 *                  of a COFF object file.
**/

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <cstring>

constexpr const char * target_section = ".text";

namespace machine_type {
    constexpr unsigned x86          = 0x014C;
    constexpr unsigned iit          = 0x0200;
    constexpr unsigned x64          = 0x8664;
}

struct IMAGE_FILE_HEADER {
    uint16_t   Machine              : 16;
    uint16_t   NumberOfSections     : 16;
    uint32_t   TimeDateStamp        : 32;
    uint32_t   PointerToSymbolTable : 32;
    uint32_t   NumberOfSymbols      : 32;
    uint16_t   SizeOfOptionalHeader : 16;
    uint16_t   Characteristics      : 16;
};

struct IMAGE_SECTION_HEADER {
    unsigned char  Name[8];
    union {
        uint32_t PhysicalAddress    : 32;
        uint32_t VirtualSize        : 32;
    } Misc;
    uint32_t   VirtualAddress       : 32;
    uint32_t   SizeOfRawData        : 32;
    uint32_t   PointerToRawData     : 32;
    uint32_t   PointerToRelocations : 32;
    uint32_t   PointerToLinenumbers : 32;
    uint16_t   NumberOfRelocations  : 16;
    uint16_t   NumberOfLinenumbers  : 16;
    uint32_t   Characteristics      : 32;
};

static inline void __attribute__((noreturn)) __usage_error(const char* msg, int exit_code) {

    std::cerr << "Usage error: " << msg << "\n";

    exit(exit_code);

}

int main(int argc, char** argv) {

    if (argc < 2) {
        __usage_error("No file specified.", 1);
    } else if (argc > 256) {
        __usage_error("Maximum number of files exceeded", 1);
    }

    --argc;
    ++argv;

    using io_pair_t = std::pair<std::string, std::string>;
    std::vector<io_pair_t> targets(argc & 0xFF);

    for (int i = 0; i < argc; ++i) {
        auto& target = targets.at(i & 0xFF);
        target.first  = argv[i];
        target.second = "out-" + std::to_string(i) + ".bin";
    }

    for (const auto& target : targets) {
        std::cout << "Target:\t" << target.first << " -> " << target.second << "\n";
        std::ifstream file(target.first, std::ios::binary);
        if (!file.is_open()) {
            __usage_error("File could not be opened.", 2);
        }

        IMAGE_FILE_HEADER file_header{};
        file.read((char *)&file_header, sizeof(file_header));

        switch (file_header.Machine) {
            [[unlikely]]
            case machine_type::x86:
                break;
            [[unlikely]]
            case machine_type::iit:
                break;
            [[likely]]
            case machine_type::x64:
                break;
            default:
                file.close();
                __usage_error("File not supported.", 3);
        }

        std::vector<IMAGE_SECTION_HEADER> section_headers(file_header.NumberOfSections);
        file.read(reinterpret_cast<char *>(section_headers.data()), file_header.NumberOfSections * sizeof(IMAGE_SECTION_HEADER));

        IMAGE_SECTION_HEADER* text_section{nullptr};
        for (const auto& section : section_headers) {
            if (std::strncmp(reinterpret_cast<const char *>(section.Name), target_section, 5) == 0) {
                text_section = (decltype(text_section))&section;
            }
        }

        if (!text_section) {
            std::cerr << "Operation failed: .text section not found." << "\n";
            file.close();
            return 4;
        }

        std::vector<unsigned char> data(text_section->SizeOfRawData);
        file.seekg(text_section->PointerToRawData, std::ios::beg);
        file.read(reinterpret_cast<char *>(data.data()), text_section->SizeOfRawData);
        file.close();

        std::cout << "Size of " << target.first << ":\t" << data.size() << "\n";

        std::ofstream outfile(target.second, std::ios::binary);
        outfile.write(reinterpret_cast<char *>(data.data()), text_section->SizeOfRawData);
        outfile.close();

        std::cout << "Written to '" << target.second << "'.\n" << std::endl;
    }

    return 0;

}
