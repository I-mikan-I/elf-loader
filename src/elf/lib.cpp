module;
#include <cstdint>
#include <ranges>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <stdexcept>
export module Elf;
import Util;
using namespace std;

namespace elf
{

    export
    {
        using Elf32_Half = uint16_t;
        using Elf32_Word = uint32_t;
        using Elf32_Addr = uint32_t;
        using Elf32_Off = uint32_t;
        using Elf64_Half = uint16_t;
        using Elf64_Word = uint32_t;
        using Elf64_Addr = uint64_t;
        using Elf64_Off = uint64_t;
    }
    enum Width
    {
        X32,
        X64
    };
    constexpr int EI_NIDENT = 16;
    template <Width W>
    struct header_t;
    template <>
    struct header_t<X32>
    {
        static constexpr size_t size = 52;
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
    static_assert(sizeof(header_t<X32>) == header_t<X32>::size);
    template <>
    struct header_t<X64>
    {
        static constexpr size_t size = 64;
        unsigned char e_ident[EI_NIDENT];
        Elf64_Half e_type;
        Elf64_Half e_machine;
        Elf64_Word e_version;
        Elf64_Addr e_entry;
        Elf64_Off e_phoff;
        Elf64_Off e_shoff;
        Elf64_Word e_flags;
        Elf64_Half e_ehsize;
        Elf64_Half e_phentsize;
        Elf64_Half e_phnum;
        Elf64_Half e_shentsize;
        Elf64_Half e_shnum;
        Elf64_Half e_shstrndx;
    };
    static_assert(sizeof(header_t<X64>) == header_t<X64>::size);

    template <Width W>
    std::ostream &operator<<(std::ostream &os, const elf::header_t<W> &header);
    export template <ranges::view V>

    class Elf
    {
    public:
        union
        {
            header_t<X32> x32;
            header_t<X64> x64;
        } header;

        Elf(V &&_file) : file{std::forward<V>(_file)}
        {
            unsigned char e_ident[EI_NIDENT];
            if (ranges::size(file) < EI_NIDENT)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), EI_NIDENT, e_ident);
            if (!ranges::equal(e_ident | views::take(4), (unsigned char[]){0x7f, 'E', 'L', 'F'}))
            {
                throw std::invalid_argument("ELF header invalid");
            }
            if (e_ident[4] == 1)
            {
                Initialize32();
            }
            else
            {
                Initialize64();
            }
        }

    private:
        V file;
        Width width;
        void Initialize32()
        {
            if (ranges::size(file) < header.x32.size)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), header.x32.size, reinterpret_cast<unsigned char *>(&(this->header.x32)));
            std::cout << header.x32;
        }
        void Initialize64()
        {
            if (ranges::size(file) < header.x64.size)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), header.x64.size, reinterpret_cast<unsigned char *>(&(this->header.x64)));
            std::cout << header.x64;
        }
    };

    template <Width W>
    std::ostream &operator<<(std::ostream &os, const elf::header_t<W> &header)
    {
        if constexpr (W == X32)
        {
            os << "HEADER (X32)";
        }
        else if constexpr (W == X64)
        {
            os << "HEADER (X64)";
        }
        else
        {
            static_assert(false);
        }
        os << "\n{" << std::setfill(' ') << std::right << '\n';
        os << std::setw(15) << "e_ident:";
        for (auto &c : header.e_ident)
        {
            os << " 0x" << hex << static_cast<unsigned int>(c);
        }
        os << '\n';
        os << std::setw(15) << "e_type:" << ' ' << header.e_type << '\n';
        os << std::setw(15) << "e_machine:" << ' ' << header.e_machine << '\n';
        os << std::setw(15) << "e_version:" << ' ' << header.e_version << '\n';
        os << std::setw(15) << "e_entry:"
           << " 0x" << hex << header.e_entry << '\n';
        os << std::setw(15) << "e_phoff:"
           << " 0x" << hex << header.e_phoff << '\n';
        os << std::setw(15) << "e_shoff:"
           << " 0x" << hex << header.e_shoff << '\n';
        os << std::setw(15) << "e_flags:"
           << " 0x" << hex << header.e_flags << '\n';
        os << std::setw(15) << "e_ehsize:" << ' ' << dec << header.e_ehsize << '\n';
        os << std::setw(15) << "e_phentsize:" << ' ' << dec << header.e_phentsize << '\n';
        os << std::setw(15) << "e_phnum:" << ' ' << dec << header.e_phnum << '\n';
        os << std::setw(15) << "e_shentsize:" << ' ' << dec << header.e_shentsize << '\n';
        os << std::setw(15) << "e_shnum:" << ' ' << dec << header.e_shnum << '\n';
        os << std::setw(15) << "e_shstrndx:" << ' ' << dec << header.e_shstrndx << '\n';

        return os << "}\n";
    }
}