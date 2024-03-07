module;
#include <cstdint>
export module Elf;
using namespace std;

export
{
    using Elf32_Half = uint16_t;
    using Elf32_Word = uint32_t;
    using Elf32_Addr = uint32_t;
    using Elf32_Off = uint32_t;
}

constexpr int EI_NIDENT = 16;

class ElfHeader
{
public:
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
};