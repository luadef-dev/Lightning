#pragma once

#include <Windows.h>
#include <exception>

#include "minhook/include/MinHook.h"

namespace Hooks
{
    namespace Originals
    {
        inline decltype(&MessageBoxA) oMessageBoxA;

        inline decltype(&CreateProcessW) oCreateProcessW;
        inline decltype(&CreateDirectoryW) oCreateDirectoryW;
        inline decltype(&CreateFileW) oCreateFileW;

        inline decltype(&FindFirstFileW) oFindFirstFileW;
        inline decltype(&FindFirstFileExW) oFindFirstFileExW;

        inline decltype(&GetFileAttributesExW) oGetFileAttributesExW;

        inline decltype(&WriteFile) oWriteFile;

        inline decltype(&FindResourceA) oFindResourceA;
        inline decltype(&SizeofResource) oSizeofResource;
        inline decltype(&LoadResource) oLoadResource;
        inline decltype(&LockResource) oLockResource;
    }

    namespace List
    {
        int WINAPI MessageBoxA_Hooked(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
    
        HANDLE WINAPI FindFirstFileW_Hooked(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
        HANDLE WINAPI FindFirstFileExW_Hooked(LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData,
            FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags);

        BOOL WINAPI GetFileAttributesExW_Hooked(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation);

        HANDLE WINAPI CreateFileW_Hooked(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
            LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
    
        BOOL WINAPI CreateDirectoryW_Hooked(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
        BOOL WINAPI CreateProcessW_Hooked(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
            LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
            LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
        BOOL WINAPI WriteFile_Hooked(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                                     LPOVERLAPPED lpOverlapped);

        HRSRC WINAPI FindResourceA_Hooked(HMODULE hModule, LPCSTR lpName,
                                          LPCSTR lpType);
        DWORD WINAPI SizeofResource_Hooked(HMODULE hModule, HRSRC hResInfo);
        HGLOBAL WINAPI LoadResource_Hooked(HMODULE hModule, HRSRC hResInfo);
        LPVOID WINAPI LockResource_Hooked(HGLOBAL hResData);
    }

    void Start();
    void ShutDown();
}