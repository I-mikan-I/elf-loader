module;
#include <cstdint>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>
#include <stdexcept>
#include <format>
#include <vector>
export module Elf;
import :Header;
import :Util;
import :Pheader;
import Util;
using namespace std;

namespace elf
{
    export template <ranges::view V>
        requires ranges::contiguous_range<V>
    class Elf
    {
    public:
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
        Elf(Elf &other) = delete;
        Elf(Elf &&other) = default;
        Elf &operator=(Elf &other) = delete;
        Elf &operator=(Elf &&other) = default;
        void PrintProgramHeaders() const
        {
            if (width == X32)
            {
                for (auto &h : GetProgramHeaders<X32>())
                {
                    std::cout << h;
                }
            }
            else if (width == X64)
            {
                for (auto &h : GetProgramHeaders<X64>())
                {
                    std::cout << h;
                }
            }
        }

    private:
        V file;
        Width width;
        union
        {
            header_t<X32> x32;
            header_t<X64> x64;
        } header;
        template <Width W>
        const header_t<W> &GetHeader() const
        {
            if constexpr (W == X32)
            {
                return header.x32;
            }
            else if constexpr (W == X64)
            {
                return header.x64;
            }
            else
            {
                static_assert(false);
            }
        }
        template <Width W>
        ranges::view auto GetProgramHeaders() const
        {
            const unsigned char *data = ranges::data(file);
            const ProgramHeader<W> *begin = reinterpret_cast<const ProgramHeader<W> *>(data + GetHeader<W>().e_phoff);
            if (GetHeader<W>().e_phentsize != sizeof(ProgramHeader<W>))
            {
                throw std::invalid_argument(std::format("Unexpected Program Header size: {}", sizeof(ProgramHeader<W>)));
            }
            return ranges::subrange(begin, begin + GetHeader<W>().e_phnum);
        }
        void Initialize32()
        {
            if (ranges::size(file) < header.x32.size)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), header.x32.size, reinterpret_cast<unsigned char *>(&(this->header.x32)));
            debug << header.x32;
            width = X32;
        }
        void Initialize64()
        {
            if (ranges::size(file) < header.x64.size)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), header.x64.size, reinterpret_cast<unsigned char *>(&(this->header.x64)));
            debug << header.x64;
            width = X64;
        }
    };

}