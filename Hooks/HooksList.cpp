#include "../PE/Bytes/PEByte.h"
#include "../Utilities/Utilities.h"
#include "../Globals.h"

#include "Hooks.h"
#include "HooksPathes.h"

int WINAPI Hooks::List::MessageBoxA_Hooked(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    if (strstr(lpCaption, xorstr_("loader")))
    {
        lpCaption = Globals::szMessagePrefix.c_str();

        if (strstr(lpText, xorstr_("press OK in game menu")))
        {
            lpText = xorstr_("fuck around and find out =)");

            while (!Globals::hModuleNavSystem)
                continue;

#if _DEBUG
            Utilities::CreateConsoleMessage("allocated binary, requested: (" + std::string(lpText) + ")!\n");
#endif

            return EXIT_SUCCESS;
        }
    }

    return Originals::oMessageBoxA(hWnd, lpText, lpCaption, uType);
}

HANDLE WINAPI Hooks::List::FindFirstFileW_Hooked(
    LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) 
{
    std::wstring wszActualPath = lpFileName ? std::wstring(lpFileName) : L"";

    if (!wszActualPath.empty()) 
    {
        std::wstring wszRedirected = HooksPathes::RedirectPath(wszActualPath);
     
        if (wszRedirected != wszActualPath)
        {
            HooksPathes::CreateTargetDirectory(wszRedirected);
            wszActualPath = wszRedirected;
        }
    }

    HANDLE hFind = Originals::oFindFirstFileW(
        wszActualPath.empty() ? nullptr : wszActualPath.c_str(), lpFindFileData);

    return hFind;
}

HANDLE WINAPI Hooks::List::FindFirstFileExW_Hooked(
    LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData,
    FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter,
    DWORD dwAdditionalFlags)
{
    std::wstring wszActualPath = lpFileName ? std::wstring(lpFileName) : L"";

    if (!wszActualPath.empty()) 
    {
        std::wstring wszRedirected = HooksPathes::RedirectPath(wszActualPath);
      
        if (wszRedirected != wszActualPath) 
        {
            HooksPathes::CreateTargetDirectory(wszRedirected);
            wszActualPath = wszRedirected;
        }
    }

    HANDLE hFind = Originals::oFindFirstFileExW(
        wszActualPath.empty() ? nullptr : wszActualPath.c_str(), fInfoLevelId,
        lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

    return hFind;
}

BOOL WINAPI Hooks::List::GetFileAttributesExW_Hooked(
    LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
    LPVOID lpFileInformation)
{
    std::wstring wszActualPath = lpFileName ? std::wstring(lpFileName) : L"";

    if (!wszActualPath.empty()) 
    {
        std::wstring wszRedirected = HooksPathes::RedirectPath(wszActualPath);

        if (wszRedirected != wszActualPath)
            wszActualPath = wszRedirected;
    }

    BOOL result = Originals::oGetFileAttributesExW(
        wszActualPath.empty() ? nullptr : wszActualPath.c_str(), fInfoLevelId,
        lpFileInformation);

    return result;
}

HANDLE WINAPI Hooks::List::CreateFileW_Hooked(
    LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) 
{
    std::wstring wszActualPath = lpFileName ? std::wstring(lpFileName) : L"";

    if (!wszActualPath.empty()) 
    {
        std::wstring wszRedirected = HooksPathes::RedirectPath(wszActualPath);
       
        if (wszRedirected != wszActualPath)
        {
            HooksPathes::CreateTargetDirectory(wszRedirected);
            wszActualPath = wszRedirected;
        }
    }

    HANDLE hFile = Originals::oCreateFileW(
        wszActualPath.empty() ? nullptr : wszActualPath.c_str(),
        dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

    return hFile;
}

BOOL WINAPI Hooks::List::CreateDirectoryW_Hooked(
    LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) 
{
    if (lpPathName)
    {
        std::wstring wszFilePath(lpPathName);
        std::wstring wszRedirected = HooksPathes::RedirectPath(wszFilePath);
  
        if (wszRedirected != wszFilePath)
            return TRUE;
    }

    BOOL result = Originals::oCreateDirectoryW(lpPathName, lpSecurityAttributes);
    return result;
}

BOOL WINAPI Hooks::List::CreateProcessW_Hooked(
    LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
    DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
    if (!lpCommandLine)
        return Originals::oCreateProcessW(
            lpApplicationName, lpCommandLine, lpProcessAttributes,
            lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
            lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

    std::wstring szwCommandLine = lpCommandLine;

    std::transform(szwCommandLine.begin(), szwCommandLine.end(),
                   szwCommandLine.begin(), ::towlower);

    bool bHasTelegram = false;

    if (PEByte::UserData::wszTelegram)
    {
        std::wstring wszTelegramStr;

        for (const auto &part : *PEByte::UserData::wszTelegram)
            wszTelegramStr += part;

        bHasTelegram = szwCommandLine.find(wszTelegramStr.c_str()) != std::wstring::npos;
    }

    if (bHasTelegram)
        return EXIT_SUCCESS;

    return Originals::oCreateProcessW(
        lpApplicationName, lpCommandLine, lpProcessAttributes,
        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
        lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

BOOL WINAPI Hooks::List::WriteFile_Hooked(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped)
{
    if (lpBuffer && nNumberOfBytesToWrite > 0)
    {
        std::string szlpBuffer(reinterpret_cast<const char*>(lpBuffer), nNumberOfBytesToWrite);

        std::string szSerchAuthor = xorstr_("\"author\": \"1111111111\"");
        std::string szSerchAuthor2 = xorstr_("\"author\": \"1111111111\"");

        std::string szSerchDate = xorstr_("\"creation_date\": \"unknown\"");
        std::string szSerchDate2 = xorstr_("\"creation_date\": \"Unknown\"");

        size_t dwAuthorPos1 = szlpBuffer.find(szSerchAuthor);
        size_t dwAuthorPos2 = szlpBuffer.find(szSerchAuthor2);

        size_t dwDatePos1 = szlpBuffer.find(szSerchDate);
        size_t dwDatePos2 = szlpBuffer.find(szSerchDate2);

        bool bShouldReplace = false;

        if (dwAuthorPos1 != std::string::npos || dwAuthorPos2 != std::string::npos)
            bShouldReplace = true;

        if (dwDatePos1 != std::string::npos || dwDatePos2 != std::string::npos)
            bShouldReplace = true;

        if (bShouldReplace)
        {
            std::string szModifiedBuffer = szlpBuffer;

            time_t tt;
            time(&tt);

            struct tm* ti = localtime(&tt);

            char szTimeBuffer[80];
            strftime(szTimeBuffer, sizeof(szTimeBuffer), xorstr_("%Y-%m-%d %H:%M:%S"), ti);

            std::string szNewAuthor = xorstr_("\"author\": \"") + std::string(Globals::UserData::szUserName) + xorstr_("\"");

            if (dwAuthorPos1 != std::string::npos)
                szModifiedBuffer.replace(dwAuthorPos1, szSerchAuthor.length(), szNewAuthor);

            if (dwAuthorPos2 != std::string::npos)
            {
                dwAuthorPos2 = szModifiedBuffer.find(szSerchAuthor2);

                if (dwAuthorPos2 != std::string::npos)
                    szModifiedBuffer.replace(dwAuthorPos2, szSerchAuthor2.length(), szNewAuthor);
            }

            std::string szNewDate = xorstr_("\"creation_date\": \"") + std::string(szTimeBuffer) + xorstr_("\"");

            if (dwDatePos1 != std::string::npos)
            {
                dwDatePos1 = szModifiedBuffer.find(szSerchDate);

                if (dwDatePos1 != std::string::npos)
                    szModifiedBuffer.replace(dwDatePos1, szSerchDate.length(), szNewDate);
            }

            if (dwDatePos2 != std::string::npos)
            {
                dwDatePos2 = szModifiedBuffer.find(szSerchDate2);

                if (dwDatePos2 != std::string::npos)
                    szModifiedBuffer.replace(dwDatePos2, szSerchDate2.length(), szNewDate);
            }

            BOOL bReturn = Originals::oWriteFile(hFile, szModifiedBuffer.c_str(), static_cast<DWORD>(szModifiedBuffer.size()),
                lpNumberOfBytesWritten, lpOverlapped);

            return bReturn;
        }
    }

    return Originals::oWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

HRSRC WINAPI Hooks::List::FindResourceA_Hooked(HMODULE hModule, LPCSTR lpName, LPCSTR lpType)
{
    HRSRC hReturn = Originals::oFindResourceA(hModule, lpName, lpType);

    if (strstr(lpType, xorstr_("DEFAULT_AVATAR"))) 
    {
        Globals::UserData::hResInfoAvatar = hReturn;

#if _DEBUG
        Utilities::CreateConsoleMessage("finded default avatar\n");
#endif
    }

    return hReturn;
}

DWORD WINAPI Hooks::List::SizeofResource_Hooked(HMODULE hModule, HRSRC hResInfo)
{
    if (hResInfo == Globals::UserData::hResInfoAvatar) 
    {
        size_t dwSize = Globals::UserData::VecAvatar.size();

        if (dwSize)
        {
#if _DEBUG
            Utilities::CreateConsoleMessage("reparsed size avatar (" + std::to_string(dwSize) + ")\n");
#endif

            return static_cast<DWORD>(dwSize);
        }
    }

    return Originals::oSizeofResource(hModule, hResInfo);
}

HGLOBAL WINAPI Hooks::List::LoadResource_Hooked(HMODULE hModule, HRSRC hResInfo) 
{
    HGLOBAL hGlobal = Originals::oLoadResource(hModule, hResInfo);

    if (hResInfo == Globals::UserData::hResInfoAvatar)
        Globals::UserData::hGlobalAvatar = hGlobal;

    return hGlobal;
}

LPVOID WINAPI Hooks::List::LockResource_Hooked(HGLOBAL hResData)
{
    if (hResData == Globals::UserData::hGlobalAvatar)
    {
        uint8_t* uAvatar = Globals::UserData::VecAvatar.data();

        if (uAvatar)
        {
#if _DEBUG
            Utilities::CreateConsoleMessage("loaded avatar!\n");
#endif

            return uAvatar;
        }
    }

    return Originals::oLockResource(hResData);
}