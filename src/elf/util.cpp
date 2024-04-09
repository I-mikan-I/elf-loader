module;
#include <cstdint>
export module Elf:Util;
namespace elf {
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
  using Elf64_Sword = int32_t;
  using Elf64_Xword = uint64_t;
  using Elf64_Sxword = int64_t;
  enum Width
  {
    X32,
    X64
  };
}
}