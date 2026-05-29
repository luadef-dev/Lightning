#pragma once

#include <Windows.h>
#include <cstdint>
#include <vector>

#include "Process/Process.h"

namespace Memory
{
    bool ReplaceTextInPattern(blackbone::Process &bProcess, const char *szText, blackbone::ptr_t bAddress, size_t dwOldTextLength = 0);
    bool FindPattern(blackbone::Process &bProcess, blackbone::ptr_t bAddress, std::vector<std::uint8_t> szPattern, size_t dwSearchSize = 0x1000);
}