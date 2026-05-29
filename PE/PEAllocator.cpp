#include "../Globals.h"
#include "../Memory/Memory.h"
#include "../Utilities/Utilities.h"

#include "Bytes/PEByte.h"
#include "PEAllocator.h"

void PEAllocator::Fix()
{
    std::size_t dwDataSize = sizeof(PEByte::szStart) +
                             HelperData::pBinary.dwSize + sizeof(PEByte::szEnd);

    std::uint8_t *pNewBuffer = new std::uint8_t[dwDataSize];

    // reallocate binary header/footer
    std::memcpy(pNewBuffer, PEByte::szStart, sizeof(PEByte::szStart));

    std::memcpy(pNewBuffer + sizeof(PEByte::szStart),
                HelperData::pBinary.Buffer, HelperData::pBinary.dwSize);
    std::memcpy(pNewBuffer + sizeof(PEByte::szStart) +
                    HelperData::pBinary.dwSize,
                PEByte::szEnd, sizeof(PEByte::szEnd));

#if _DEBUG
    Utilities::CreateConsoleMessage("fixed binary pe, " +
                                    std::to_string(dwDataSize) + "!\n");
#endif

    // remove old binary
    delete[] HelperData::pBinary.Buffer;

    // set new binary
    HelperData::pBinary.Buffer = pNewBuffer;
    HelperData::pBinary.dwSize = dwDataSize;
}

blackbone::call_result_t<blackbone::ModuleDataPtr> PEAllocator::Map() 
{
    auto mapCallback = [](blackbone::CallbackType cType, void *, blackbone::Process &, const blackbone::ModuleData &ModuleInfo)
        {
            if (cType == blackbone::CallbackType::PreCallback) 
            {
                if (blackbone::Utils::ToLower(ModuleInfo.name) == xorstr_(L"user32.dll"))
                    return blackbone::LoadData(blackbone::MT_Native, blackbone::Ldr_Ignore);
            }

            return blackbone::LoadData(blackbone::MT_Default, blackbone::Ldr_Ignore);
        };

    auto pResult = HelperData::bProcess.mmap().MapImage(HelperData::pBinary.dwSize, HelperData::pBinary.Buffer, 
        false, blackbone::eLoadFlags::NoDelayLoad | blackbone::eLoadFlags::NoTLS, mapCallback);

    // remove binary in memory
    if (HelperData::pBinary.Buffer) 
    {
        delete[] HelperData::pBinary.Buffer;
        HelperData::pBinary.Buffer = nullptr;
    }

    return pResult;
}

void PEAllocator::ReadyCS2()
{
#if _DEBUG
    Utilities::CreateConsoleMessage("waiting for navsystem...\n");
#endif

    while (true)
    {
        auto &Modules = HelperData::bProcess.modules().GetAllModules();
        
        for (const auto &Module : Modules)
        {
            if (blackbone::Utils::ToLower(Module.first.first).find(xorstr_(L"navsystem")) != std::wstring::npos) 
            {
                Globals::hModuleNavSystem =
                    reinterpret_cast<HMODULE>(Module.second->baseAddress);

#if _DEBUG
                Utilities::CreateConsoleMessage("found navsystem at: 0x" + 
                    std::to_string(reinterpret_cast<uintptr_t>(Globals::hModuleNavSystem)) + "\n");
#endif
                return;
            }
        }
    }
}

void PEAllocator::AllocateUserData()
{
    static uintptr_t dwUsername = HelperData::BinaryBaseAddress + 0x3B60E0;
    static uintptr_t dwSubLeft = HelperData::BinaryBaseAddress + 0x3B6100;

    { // username
        std::vector<std::uint8_t> VecPattern;

        for (const auto &ch : PEByte::UserData::szUsername)
            VecPattern.push_back(static_cast<std::uint8_t>(ch[0]));

        while (!Memory::FindPattern(HelperData::bProcess, dwUsername, VecPattern, VecPattern.size()))
            continue;

        if (!Memory::ReplaceTextInPattern(
                HelperData::bProcess, Globals::UserData::szUserName.c_str(), dwUsername, VecPattern.size()))
            throw std::exception(xorstr_("Failed to allocate user data #1 =("));

        VecPattern.clear();
    }

    { // sub
        std::vector<std::uint8_t> VecPattern;

        for (const auto &ch : PEByte::UserData::szSub)
            VecPattern.push_back(static_cast<std::uint8_t>(ch[0]));

        while (!Memory::FindPattern(HelperData::bProcess, dwSubLeft, VecPattern, VecPattern.size()))
            continue;

        if (!Memory::ReplaceTextInPattern(
                HelperData::bProcess, Globals::UserData::szSubLeft.c_str(), dwSubLeft, VecPattern.size()))
            throw std::exception(xorstr_("Failed to allocate user data #2 =("));

        VecPattern.clear();
    }

#if _DEBUG
    Utilities::CreateConsoleMessage("allocated user data!\n");
#endif
}

bool PEAllocator::Attach() 
{
    char szErrorMsg[256];

    NTSTATUS pStatus = HelperData::bProcess.Attach(GetCurrentProcessId(), PROCESS_ALL_ACCESS);

    if (NT_ERROR(pStatus)) 
    {
        sprintf_s(szErrorMsg, xorstr_("Failed attach to process\nError: 0x%X"), pStatus);
        Utilities::CreateErrorMessage(szErrorMsg);
        return false;
    }

    return true;
}

void PEAllocator::Start() 
{
    char szErrorMsg[256];

    // load image
    if (!Resources::LoadResource(Resources::eResources::IDR_BINARY, xorstr_("BINARY"), HelperData::pBinary))
        throw std::exception(xorstr_("Failed to get binary =("));

    if (HelperData::pBinary.dwSize <= 0)
        throw std::exception(xorstr_("Invalid binary size :("));

    // fix image after load to game
    PEAllocator::Fix();

    // load
    auto Status = PEAllocator::Map();

    if (!Status.success())
    {
        sprintf_s(szErrorMsg, xorstr_("Failed to allocate binary -> 0x%X"), Status.status);
        throw std::exception(szErrorMsg);
    }
#if _DEBUG
    else
        Utilities::CreateConsoleMessage("binary mapped !\n");
#endif

    HelperData::BinaryBaseAddress = 0x710000000000;
}