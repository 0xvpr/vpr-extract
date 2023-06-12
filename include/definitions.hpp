#ifndef   DEFINITIONS_HEADER
#define   DEFINITIONS_HEADER

#include <cstdint>

#ifndef   PACKED

  #ifdef    __GNUC__
    #define PACKED [[gnu::packed]]
  #endif // __GNUC__

  /**
   * TODO ifdef _WIN32
  **/

#endif // PACKED

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
    unsigned char   e_magic[16];
    uint16_t        e_type;
    uint16_t        e_machine;
    uint32_t        e_version;
    uint32_t        e_entry;
    uint32_t        e_phoff;
    uint32_t        e_shoff;
    uint32_t        e_flags;
    uint16_t        e_ehsize;
    uint16_t        e_phentsize;
    uint16_t        e_phnum;
    uint16_t        e_shentsize;
    uint16_t        e_shnum;
    uint16_t        e_shstrndx;
} ElfHeader32;

typedef struct _ElfHeader64 {
    unsigned char   e_magic[16];
    uint16_t        e_type;
    uint16_t        e_machine;
    uint32_t        e_version;
    uint64_t        e_entry;
    uint64_t        e_phoff;
    uint64_t        e_shoff;
    uint32_t        e_flags;
    uint16_t        e_ehsize;
    uint16_t        e_phentsize;
    uint16_t        e_phnum;
    uint16_t        e_shentsize;
    uint16_t        e_shnum;
    uint16_t        e_shstrndx;
} ElfHeader64;

typedef struct _ElfSectionHeader64 {
    uint32_t        sh_name;      /* Section name */
    uint32_t        sh_type;      /* Section type */
    uint64_t        sh_flags;     /* Section attributes */
    uint64_t        sh_addr;      /* Virtual address in memory */
    uint64_t        sh_offset;    /* Offset in file */
    uint64_t        sh_size;      /* Size of section */
    uint32_t        sh_link;      /* Link to other section */
    uint32_t        sh_info;      /* Miscellaneous information */
    uint64_t        sh_addralign; /* Address alignment boundary */
    uint64_t        sh_entsize;   /* Size of entries, if section has table */
} ElfSectionHeader64;
} // namespace definitions

#endif // DEFINITIONS_HEADER
