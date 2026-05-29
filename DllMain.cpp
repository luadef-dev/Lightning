#include "EntryPoint/EntryPoint.h"
#include "Hooks/Hooks.h"
#include "PE/PEAllocator.h"
#include "Utilities/Utilities.h"
#include "Globals.h"

static LONG CALLBACK VehHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
    if (std::filesystem::is_regular_file(Globals::Pathes::szCrashExceptionFilePath))
        std::filesystem::remove(Globals::Pathes::szCrashExceptionFilePath);

    PEXCEPTION_RECORD pRecord = ExceptionInfo->ExceptionRecord;

    std::ofstream Log(Globals::Pathes::szCrashExceptionFilePath.c_str(), std::ios::app);

    if (Log.is_open())
    {
        Log << xorstr_("=== Crash Dump -> VehHandler ===\n");
        Log << xorstr_("Code: 0x") << std::hex << pRecord->ExceptionCode << xorstr_("\n");
        Log << xorstr_("Address: ") << pRecord->ExceptionAddress << xorstr_("\n");

        CONTEXT *Ctx = ExceptionInfo->ContextRecord;
        Log << xorstr_("RIP: 0x") << std::hex << Ctx->Rip << xorstr_("\n");
        Log << xorstr_("RSP: 0x") << std::hex << Ctx->Rsp << xorstr_("\n");
        Log << xorstr_("RAX: 0x") << std::hex << Ctx->Rax << xorstr_("\n");

        Log << xorstr_("========================\n");
        Log << xorstr_("Send to telegram of the (") << Globals::szMessagePrefix << xorstr_(")!");

        Log.close();
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

static LONG WINAPI TopLevelVehHandler(PEXCEPTION_POINTERS ExceptionInfo) 
{
    if (std::filesystem::is_regular_file(Globals::Pathes::szCrashTopLevelExceptionFilePath))
        std::filesystem::remove(Globals::Pathes::szCrashTopLevelExceptionFilePath);

    PEXCEPTION_RECORD pRecord = ExceptionInfo->ExceptionRecord;

    std::ofstream Log(Globals::Pathes::szCrashTopLevelExceptionFilePath.c_str(), std::ios::app);

    if (Log.is_open()) 
    {
        Log << xorstr_("=== Crash Dump -> TopLevelVehHandler ===\n");
        Log << xorstr_("Code: 0x") << std::hex << pRecord->ExceptionCode << xorstr_("\n");
        Log << xorstr_("Address: ") << pRecord->ExceptionAddress << xorstr_("\n");

        CONTEXT *Ctx = ExceptionInfo->ContextRecord;
        Log << xorstr_("RIP: 0x") << std::hex << Ctx->Rip << xorstr_("\n");
        Log << xorstr_("RSP: 0x") << std::hex << Ctx->Rsp << xorstr_("\n");
        Log << xorstr_("RAX: 0x") << std::hex << Ctx->Rax << xorstr_("\n");

        Log << xorstr_("========================\n");
        Log << xorstr_("Send to telegram of the (") << Globals::szMessagePrefix << xorstr_(")!");

        Log.close();
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        Globals::hModule = hModule;

        EntryPoint::InitPathes();

        DisableThreadLibraryCalls(Globals::hModule);

        AddVectoredExceptionHandler(TRUE, VehHandler);
        SetUnhandledExceptionFilter(TopLevelVehHandler);

        if (!PEAllocator::Attach())
            return FALSE;

#ifndef _DEBUG
        Utilities::EnableDebugPrevilegies();
#endif

        blackbone::Thread *bThread = PEAllocator::HelperData::bProcess.threads().CreateNew(
            reinterpret_cast<blackbone::ptr_t>(&EntryPoint::Start), 0).result_data->get();

        if (bThread->valid())
            bThread->Close();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        EntryPoint::ShutDown();

    return TRUE;
}