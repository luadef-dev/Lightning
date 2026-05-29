#include "../Hooks/Hooks.h"
#include "../Globals.h"

#include "Utilities.h"

bool Utilities::FileManager::AddDefaultCloudConfigToFolder(std::string_view szFile,
    Resources::eResources iResource, std::string_view szNameBinary)
{
    Resources::DataInfo_t CfgResource{};

    if (!Resources::LoadResource(iResource, szNameBinary.data(), CfgResource))
        return false;

    if (CfgResource.dwSize < 0)
        return false;

    std::string szConfigPath = Globals::Pathes::szConfigsPath + "\\" + szFile.data();

    // check if alr added config
    if (std::filesystem::is_regular_file(szConfigPath))
        return true;

    std::ofstream CfgOut;
    CfgOut.open(szConfigPath);

    if (!CfgOut.is_open())
        return false;

    CfgOut.write(reinterpret_cast<const char*>(CfgResource.Buffer), CfgResource.dwSize);
    CfgOut.close();

#if _DEBUG
    CreateConsoleMessage("added default cloud config, (" + std::string(szFile.data()) + ")!\n");
#endif

    return true;
}

bool Utilities::FileManager::CreateDirectoryCustom(std::string_view szPath)
{
    if (!std::filesystem::is_directory(szPath))
    {
        if (!std::filesystem::create_directory(szPath))
            return false;

#if _DEBUG
        CreateConsoleMessage("created directory -> " + std::string(szPath) + "!\n");
#endif
    }

    return true;
}

// @credits: violanes
bool Utilities::FileManager::OpenBinary(std::string_view szPath, std::vector<std::uint8_t> &VecData)
{
    std::ifstream StrFile(szPath.data(), std::ios::binary);

    if (!StrFile.is_open())
        return false;

    StrFile.unsetf(std::ios::skipws);
    StrFile.seekg(0, std::ios::end);

    const auto iSize = StrFile.tellg();

    StrFile.seekg(0, std::ios::beg);

    VecData.reserve(static_cast<uint32_t>(iSize));
    VecData.insert(VecData.begin(),
                   std::istream_iterator<std::uint8_t>(StrFile),
                   std::istream_iterator<std::uint8_t>());

    StrFile.close();

    return true;
}

#ifndef _DEBUG
bool Utilities::EnableDebugPrevilegies()
{
    HANDLE hToken = nullptr;
    TOKEN_PRIVILEGES tkp{};

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        throw std::exception(xorstr_("Failed to initialize #1 #1"));

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);

    if (hToken)
        CloseHandle(hToken);

    return GetLastError() != ERROR_SUCCESS;
}
#endif

int Utilities::CreateErrorMessage(std::string_view szStr)
{
    //ShowWindow(GetActiveWindow(), SW_HIDE);
    return Hooks::Originals::oMessageBoxA(0, szStr.data(), Globals::szMessagePrefix.c_str(), MB_OK | MB_ICONERROR);
}

int Utilities::CreateInformationMessage(std::string_view szStr) {
    return Hooks::Originals::oMessageBoxA(0, szStr.data(), Globals::szMessagePrefix.c_str(), MB_OK | MB_ICONINFORMATION);
}

#if _DEBUG
bool Utilities::CreateConsole()
{
    if (!AllocConsole())
        throw std::exception("Failed to allocate debug console");

    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
    freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);

    return true;
}

void Utilities::CreateConsoleMessage(std::string_view szStr) {
    printf("< %s > -> %s", Globals::szMessagePrefix.c_str(), szStr.data());
}
#endif