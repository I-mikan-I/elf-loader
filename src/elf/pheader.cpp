module;
#include <cstdint>
#include <iostream>
#include <iomanip>
export module Elf:Pheader;
import :Util;
using namespace std;

namespace elf
{
    template <Width W>
    struct ProgramHeader;

    constexpr uint32_t PT_NULL = 0;
    constexpr uint32_t PT_LOAD = 1;
    constexpr uint32_t PT_DYNAMIC = 2;
    constexpr uint32_t PT_INTERP = 3;
    constexpr uint32_t PT_NOTE = 4;
    constexpr uint32_t PT_SHLIB = 5;
    constexpr uint32_t PT_PHDR = 6;
    constexpr uint32_t PT_TLS = 7;
    constexpr uint32_t PT_LOOS = 0x60000000;
    constexpr uint32_t PT_GNU_EH_FRAME = 0x6474e550;
    constexpr uint32_t PT_GNU_STACK = 0x6474e551;
    constexpr uint32_t PT_GNU_RELRO = 0x6474e552;
    constexpr uint32_t PT_HIOS = 0x6fffffff;
    constexpr uint32_t PT_LOPROC = 0x70000000;
    constexpr uint32_t PT_HIPROC = 0x7fffffff;

    struct p_type_t
    {
        explicit p_type_t(uint32_t v) : val{v} {};
        operator uint32_t() const { return val; }

    private:
        uint32_t val;
    };
    static_assert(sizeof(p_type_t) == sizeof(uint32_t));
    static_assert(alignof(p_type_t) == alignof(uint32_t));

    std::ostream &operator<<(std::ostream &os, const p_type_t &p_type)
    {
        auto val = static_cast<uint32_t>(p_type);
        switch (val)
        {
        case PT_NULL:
            return os << "PT_NULL";
        case PT_LOAD:
            return os << "PT_LOAD";
        case PT_DYNAMIC:
            return os << "PT_DYNAMIC";
        case PT_INTERP:
            return os << "PT_INTERP";
        case PT_NOTE:
            return os << "PT_NOTE";
        case PT_SHLIB:
            return os << "PT_SHLIB";
        case PT_PHDR:
            return os << "PT_PHDR";
        case PT_TLS:
            return os << "PT_TLS";
        case PT_GNU_EH_FRAME:
            return os << "PT_GNU_EH_FRAME";
        case PT_GNU_STACK:
            return os << "PT_GNU_STACK";
        case PT_GNU_RELRO:
            return os << "PT_GNU_RELRO";
        }
        if (val >= PT_LOOS && val <= PT_HIOS)
        {
            return os << "PT_OS: 0x" << hex << val;
        }
        if (val >= PT_LOPROC && val <= PT_HIPROC)
        {
            return os << "PT_PROC: 0x" << hex << val;
        }
        return os << "UNKNOWN: 0x" << hex << val;
    }

    template <>
    struct ProgramHeader<X32>
    {
        p_type_t p_type;
        Elf32_Off p_offset;
        Elf32_Addr p_vaddr;
        Elf32_Addr p_paddr;
        Elf32_Word p_filesz;
        Elf32_Word p_memsz;
        Elf32_Word p_flags;
        Elf32_Word p_align;
    };

    template <>
    struct ProgramHeader<X64>
    {
        p_type_t p_type;
        Elf64_Word p_flags;
        Elf64_Off p_offset;
        Elf64_Addr p_vaddr;
        Elf64_Addr p_paddr;
        Elf64_Xword p_filesz;
        Elf64_Xword p_memsz;
        Elf64_Xword p_align;
    };

    template <Width W>
    std::ostream &operator<<(std::ostream &os, const ProgramHeader<W> &pheader)
    {
        if constexpr (W == X32)
        {
            os << "PROGRAM HEADER (X32)";
        }
        else if constexpr (W == X64)
        {
            os << "PROGRAM HEADER (X64)";
        }
        else
        {
            static_assert(false);
        }
        os << "\n{" << std::setfill(' ') << std::right << '\n';
        os << std::setw(15) << "p_type: " << pheader.p_type << '\n';
        os << std::setw(15) << "p_flags: " << hex << "0x" << pheader.p_flags << '\n';
        os << std::setw(15) << "p_offset: " << hex << "0x" << pheader.p_offset << '\n';
        os << std::setw(15) << "p_vaddr: " << hex << "0x" << pheader.p_vaddr << '\n';
        os << std::setw(15) << "p_paddr: " << hex << "0x" << pheader.p_paddr << '\n';
        os << std::setw(15) << "p_filesz: " << dec << pheader.p_filesz << '\n';
        os << std::setw(15) << "p_memsz: " << dec << pheader.p_memsz << '\n';
        os << std::setw(15) << "p_align: " << dec << pheader.p_align << '\n';
        return os << "}\n";
    }
}