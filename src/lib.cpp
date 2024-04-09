module;
#include <algorithm>
#include <chrono>
#include <errno.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits.h>
#include <memory>
#include <ranges>
#include <sched.h>
#include <signal.h>
#include <stdexcept>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>
export module Lib;
import Elf;
import Util;

using namespace std::ranges;

export class Loader
{
private:
  static inline auto checked(auto ret)
  {
    if (ret == -1) {
      throw std::runtime_error(std::format("Errno: {}", strerror(errno)));
    }
    return ret;
  }
  static inline auto checked(auto ret, auto printable)
  {
    if (ret == -1) {
      throw std::runtime_error(std::format("{}: {}", printable, strerror(errno)));
    }
    return ret;
  }

public:
  explicit Loader(const std::tuple<std::vector<iovec>, std::vector<iovec>>& regions)
  {
    auto ret = fork();
    if (ret == -1) {
      throw std::runtime_error(std::format("Error code when cloning child: {}", strerror(errno)));
    }
    child = ret;
    if (!child) {
      fn(std::get<1>(regions));
    }
    int status{};
    checked(waitpid(child, &status, 0), "Error during waitpid");
    if (!WIFSTOPPED(status)) {
      throw std::runtime_error("Child ptrace not set up correctly");
    }
    debug << "Trace initialized\n";

    LoadRegions(regions);
  }

private:
  pid_t child;

  void LoadRegions(const std::tuple<std::vector<iovec>, std::vector<iovec>>& regions)
  {
    auto [src_ptrs, dst_ptrs] = regions;
    if (src_ptrs.size() != dst_ptrs.size()) {
      throw std::invalid_argument("source and destination iovec count must be equal");
    }
    if (src_ptrs.size() > IOV_MAX) {
      throw std::invalid_argument(
        std::format("supplied iov count of {} is larger than IOV_MAX of {}", src_ptrs.size(), IOV_MAX));
    }
    auto written = 0;
    auto dst_iov = dst_ptrs.begin();
    auto src_iov = src_ptrs.begin();
    while (src_iov != src_ptrs.end()) {
      if (!written) {
        written = checked(process_vm_writev(child,
                                            std::to_address(src_iov),
                                            distance(src_iov, src_ptrs.end()),
                                            std::to_address(dst_iov),
                                            distance(dst_iov, dst_ptrs.end()),
                                            0));
        debug << "loaded " << written << " bytes\n";
      } else {
        written -= src_iov->iov_len;
        src_iov++;
        dst_iov++;
      }
    }
    if (written != 0) {
      throw std::runtime_error("more bytes written than requested");
    }
  }
  static int fn(const std::vector<iovec>& dst_ptrs)
  {
    const size_t page_size = sysconf(_SC_PAGE_SIZE);
    for (auto&& [addr, len] : dst_ptrs) {
      auto page_boundary = std::bit_cast<uintptr_t>(addr) & (~(page_size - 1));
      for (uintptr_t page = page_boundary; page < (uintptr_t)addr + len; page += page_size) {
        auto res = mmap((void*)page, page_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
        if (res == (void*)-1 && errno != EEXIST) {
          checked(-1, "mmap error: "); // throws exception
        }
      }
      checked(mprotect((void*)(page_boundary),
                       (len + std::bit_cast<uintptr_t>(addr) % page_size),
                       PROT_READ | PROT_WRITE | PROT_EXEC));
    }
    checked(ptrace(PTRACE_TRACEME), "Error during ptrace(PTRACE_TRACEME)");
    raise(SIGSTOP);
    while (true) {
      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(1s);
      debug << "loop\n";
    };
    return 1;
  }
};

template<elf::Width W, typename V>
std::tuple<std::vector<iovec>, std::vector<iovec>>
getLoadableRegions(const elf::Elf<V>& file)
{
  using namespace std;
  const static uint8_t NULLED[1 << 12]{ 0 };
  vector<iovec> src_ptrs;
  vector<iovec> dst_ptrs;
  auto headers = file.template GetProgramHeaders<W>();
  debug << "loading headers\n";
  int i = 0;
  for (auto const& h : headers) {
    if (h.p_type == elf::PT_LOAD) {
      debug << "Loadable Segment: " << i << '\n';
      auto contents = h.ExecutionView(file);
      size_t size = ranges::size(contents);
      debug << "In-File memory contents len: " << hex << "0x" << size << '\n';
      if (size > h.p_memsz) {
        throw runtime_error("Program header size too large");
      }
      iovec src_iov = { .iov_base = (void*)(ranges::data(contents)), .iov_len = size };
      iovec dst_iov = { .iov_base = (void*)static_cast<uintptr_t>(h.p_vaddr), .iov_len = size };
      src_ptrs.push_back(src_iov);
      dst_ptrs.push_back(dst_iov);

      char* trailing_p_vaddr = reinterpret_cast<char*>(dst_iov.iov_base) + size;
      size_t rest = h.p_memsz - size;
      while (rest) {
        size_t to_write = std::min(sizeof(NULLED), rest);
        rest -= to_write;

        iovec src_iov = { .iov_base = (void*)NULLED, .iov_len = to_write };
        iovec dst_iov = { .iov_base = reinterpret_cast<void*>(trailing_p_vaddr), .iov_len = to_write };
        src_ptrs.push_back(src_iov);
        dst_ptrs.push_back(dst_iov);
        trailing_p_vaddr += to_write;
      }
    }
    i++;
  }
  return { src_ptrs, dst_ptrs };
}

export void
load_elf(std::filesystem::path elf)
{
  std::ifstream input{ elf, std::ios_base::binary };
  std::vector<unsigned char> file{};
  copy(subrange(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>{}) |
         views::transform([](const char& c) { return std::bit_cast<unsigned char, char>(c); }),
       std::back_inserter(file));

  elf::Elf parsed((owning_view<decltype(file)>(std::move(file))));
  parsed.PrintProgramHeaders();
  auto ranges_to_load =
    parsed.width == elf::X64 ? getLoadableRegions<elf::X64>(parsed) : getLoadableRegions<elf::X32>(parsed);
  Loader loader(ranges_to_load);
}