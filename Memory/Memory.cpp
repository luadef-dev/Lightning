#include "Memory.h"
#include "../Globals.h"

bool Memory::ReplaceTextInPattern(blackbone::Process& bProcess, const char* szText, blackbone::ptr_t bAddress, size_t dwOldTextLength)
{
    if (!szText || !bAddress)
        return false;

    size_t dwNewTextLen = strlen(szText);
    
    if (dwNewTextLen == 0)
        return true;

    std::string szTextToWrite = szText;

    if (dwOldTextLength > 0 && dwNewTextLen < dwOldTextLength)
    {
        size_t dwSpacesToAdd = dwOldTextLength - dwNewTextLen;

        for (size_t i = 0; i < dwSpacesToAdd; i++)
            szTextToWrite += ' ';
    }

    DWORD dwOldProtect = 0;
    size_t dwWriteLen = szTextToWrite.length();

    NTSTATUS Status = bProcess.memory().Protect(
        bAddress, dwWriteLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);

    if (!NT_SUCCESS(Status))
        return false;

    Status = bProcess.memory().Write(bAddress, dwWriteLen, szTextToWrite.c_str());

    if (!NT_SUCCESS(Status))
        return false;

    DWORD dwTemp = 0;
    bProcess.memory().Protect(bAddress, dwWriteLen, dwOldProtect, &dwTemp);

    return true;
}

bool Memory::FindPattern(blackbone::Process &bProcess, blackbone::ptr_t bAddress, std::vector<std::uint8_t> szPattern, size_t dwSearchSize)
{
    if (szPattern.empty() || !bAddress)
        return false;

    try 
    {
        MEMORY_BASIC_INFORMATION64 mbi{};
   
        NTSTATUS Status = bProcess.memory().Query(bAddress, &mbi);
     
        if (!NT_SUCCESS(Status))
            return false;

        if (mbi.State != MEM_COMMIT)
            return false;

        if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))
            return false;

        size_t dwRegionSize = static_cast<size_t>(mbi.RegionSize);

        blackbone::ptr_t bStart = mbi.BaseAddress;
        blackbone::ptr_t bOffset = bAddress - bStart;

        if (bOffset >= dwRegionSize || szPattern.size() == 0)
            return false;

        size_t dwMaxSearchSize = (dwSearchSize == 0) ? (dwRegionSize - bOffset) : min(dwRegionSize - bOffset, dwSearchSize);

        if (dwMaxSearchSize < szPattern.size())
            return false;

        std::vector<uint8_t> VecBuffer(dwMaxSearchSize);
        Status = bProcess.memory().Read(bAddress, dwMaxSearchSize, VecBuffer.data(), false);

        if (!NT_SUCCESS(Status))
            return false;

        for (size_t i = 0; i <= dwMaxSearchSize - szPattern.size(); i++)
        {
            bool bFound = true;

            for (size_t j = 0; j < szPattern.size(); j++)
            {
                if (VecBuffer[i + j] != szPattern[j])
                {
                    bFound = false;
                    break;
                }
            }

            if (bFound)
                return true;
        }

        return false;
    } catch (...) {
        return false;
    }
}