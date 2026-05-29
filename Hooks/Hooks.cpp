#include "../Utilities/Utilities.h"
#include "../Globals.h"

#include "Hooks.h"

void Hooks::Start()
{
    if (MH_Initialize() != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hooks"));

    if (MH_CreateHookApi(xorstr_(L"user32.dll"), xorstr_("MessageBoxA"), List::MessageBoxA_Hooked, 
        reinterpret_cast<LPVOID*>(&Originals::oMessageBoxA)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #1"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("CreateProcessW"), List::CreateProcessW_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oCreateProcessW)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #2"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("CreateDirectoryW"), List::CreateDirectoryW_Hooked, 
        reinterpret_cast<LPVOID*>(&Originals::oCreateDirectoryW)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #3"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("CreateFileW"), List::CreateFileW_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oCreateFileW)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #4"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("FindFirstFileW"), List::FindFirstFileW_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oFindFirstFileW)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #5"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("FindFirstFileExW"), List::FindFirstFileExW_Hooked, 
        reinterpret_cast<LPVOID*>(&Originals::oFindFirstFileExW)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #6"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("GetFileAttributesExW"), List::GetFileAttributesExW_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oGetFileAttributesExW)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #7"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("WriteFile"), List::WriteFile_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oWriteFile)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #8"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("FindResourceA"), List::FindResourceA_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oFindResourceA)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #9"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("SizeofResource"), List::SizeofResource_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oSizeofResource)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #10"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("LoadResource"), List::LoadResource_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oLoadResource)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #11"));

    if (MH_CreateHookApi(xorstr_(L"kernel32.dll"), xorstr_("LockResource"), List::LockResource_Hooked,
        reinterpret_cast<LPVOID*>(&Originals::oLockResource)) != MH_OK)
        throw std::exception(xorstr_("Failed to initialize hook #12"));

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
        throw std::exception(xorstr_("Failed to enable hooks"));

#if _DEBUG
    Utilities::CreateConsoleMessage("inited hooks!\n");
#endif
}

void Hooks::ShutDown()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);

    MH_Uninitialize();
}