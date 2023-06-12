#ifndef   DEFINITIONS_HEADER
#define   DEFINITIONS_HEADER

#include <cstdint>

#ifndef   PACKED
  #ifdef    __GNUC__
    #define PACKED [[gnu::packed]]
  #endif // __GNUC__
  /**
   * ifdef _WIN32
  **/
#endif // PACKED

namespace object_types {
    constexpr uint32_t unsupported = 0;
    constexpr uint32_t elf64       = 1;
    constexpr uint32_t pe32        = 2;
    constexpr uint32_t pe64        = 3;
}

namespace machine_type {
    constexpr uint16_t reloc       = 0x0001;
    constexpr uint16_t x86         = 0x014C;
    constexpr uint16_t x64         = 0x8664;
}

namespace definitions {
typedef struct PACKED _IMAGE_FILE_HEADER {
    uint16_t   Machine              : 16;
    uint16_t   NumberOfSections     : 16;
    uint32_t   TimeDateStamp        : 32;
    uint32_t   PointerToSymbolTable : 32;
    uint32_t   NumberOfSymbols      : 32;
    uint16_t   SizeOfOptionalHeader : 16;
    uint16_t   Characteristics      : 16;
} IMAGE_FILE_HEADER;

typedef struct PACKED _IMAGE_SECTION_HEADER {
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
} IMAGE_SECTION_HEADER;

typedef struct PACKED _ElfHeader32 {
    uint8_t         e_magic[16];
    uint16_t        e_type          : 16;
    uint16_t        e_machine       : 16;
    uint32_t        e_version       : 32;
    uint32_t        e_entry         : 32;
    uint32_t        e_phoff         : 32;
    uint32_t        e_shoff         : 32;
    uint32_t        e_flags         : 32;
    uint16_t        e_ehsize        : 16;
    uint16_t        e_phentsize     : 16;
    uint16_t        e_phnum         : 16;
    uint16_t        e_shentsize     : 16;
    uint16_t        e_shnum         : 16;
    uint16_t        e_shstrndx      : 16;
} ElfHeader32;

typedef struct PACKED _ElfHeader64 {
    uint8_t         e_magic[16];
    uint16_t        e_type          : 16;
    uint16_t        e_machine       : 16;
    uint32_t        e_version       : 32;
    uint64_t        e_entry         : 64;
    uint64_t        e_phoff         : 64;
    uint64_t        e_shoff         : 64;
    uint32_t        e_flags         : 32;
    uint16_t        e_ehsize        : 16;
    uint16_t        e_phentsize     : 16;
    uint16_t        e_phnum         : 16;
    uint16_t        e_shentsize     : 16;
    uint16_t        e_shnum         : 16;
    uint16_t        e_shstrndx      : 16;
} ElfHeader64;

typedef struct PACKED _ElfSectionHeader64 {
    uint32_t        sh_name         : 32;   // Section name
    uint32_t        sh_type         : 32;   // Section type
    uint64_t        sh_flags        : 64;   // Section attributes
    uint64_t        sh_addr         : 64;   // Virtual address in memory
    uint64_t        sh_offset       : 64;   // Offset in file
    uint64_t        sh_size         : 64;   // Size of section
    uint32_t        sh_link         : 32;   // Link to other section
    uint32_t        sh_info         : 32;   // Miscellaneous information
    uint64_t        sh_addralign    : 64;   // Address alignment boundary
    uint64_t        sh_entsize      : 64;   // Size of entries, if section has table
} ElfSectionHeader64;
} // namespace definitions

#endif // DEFINITIONS_HEADER
