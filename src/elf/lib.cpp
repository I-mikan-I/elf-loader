module;
#include <cstdint>
#include <ranges>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <stdexcept>
export module Elf;
using namespace std;

namespace elf
{

    export
    {
        using Elf32_Half = uint16_t;
        using Elf32_Word = uint32_t;
        using Elf32_Addr = uint32_t;
        using Elf32_Off = uint32_t;
    }
    constexpr int EI_NIDENT = 16;
    export constexpr size_t HEADER_SIZE = 52;
    using header_t = struct
    {
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
    static_assert(sizeof(header_t) == HEADER_SIZE);

    std::ostream &operator<<(std::ostream &os, const elf::header_t &header);
    export template <ranges::view V>
    class Elf
    {
    public:
        header_t header;

        Elf(V &&_file) : file{std::forward<V>(_file)}
        {
            if (ranges::size(file) < HEADER_SIZE)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), HEADER_SIZE, reinterpret_cast<unsigned char *>(&(this->header)));
            if (!ValidateHeader())
            {
                throw std::invalid_argument("ELF header invalid");
            }
            std::cout << header;
        }

    private:
        V file;
        bool ValidateHeader()
        {
            if (!ranges::equal(header.e_ident | views::take(4), (unsigned char[]){0x7f, 'E', 'L', 'F'}))
            {
                return false;
            }
            return true;
        }
    };
    std::ostream &operator<<(std::ostream &os, const elf::header_t &header)
    {
        os << "HEADER"
           << "\n{" << std::setfill(' ') << std::right << '\n';
        os << std::setw(13) << "e_ident:";
        for (auto &c : header.e_ident)
        {
            os << " 0x" << hex << static_cast<unsigned int>(c);
        }
        os << '\n';
        os << std::setw(13) << "e_type:" << ' ' << header.e_type << '\n';
        os << std::setw(13) << "e_machine:" << ' ' << header.e_machine << '\n';
        os << std::setw(13) << "e_version:" << ' ' << header.e_version << '\n';
        os << std::setw(13) << "e_entry:"
           << " 0x" << hex << header.e_entry << '\n';
        os << std::setw(13) << "e_phoff:"
           << " 0x" << hex << header.e_phoff << '\n';
        os << std::setw(13) << "e_shoff:"
           << " 0x" << hex << header.e_shoff << '\n';
        os << std::setw(13) << "e_flags:"
           << " 0x" << hex << header.e_flags << '\n';

        return os << "\n";
    }
}