#include "HooksPathes.h"

std::wstring HooksPathes::GetOldMainFull()
{
    std::wstring wszResult;

    for (const auto &part : PEByte::Pathes::wszOldMain)
        wszResult += part;

    return wszResult;
}

std::wstring HooksPathes::GetNewMainFull()
{
    std::string szMainPathFull;

    for (const auto &part : PEByte::Pathes::szMainPath)
        szMainPathFull += part;

    std::wstring wszResult =
        std::wstring(szMainPathFull.begin(), szMainPathFull.end());

    return wszResult;
}

std::wstring HooksPathes::GetOldConfigsFull() 
{
    std::wstring wszResult = GetOldMainFull();

    for (const auto &part : PEByte::Pathes::wszOldConfigsPrefix)
        wszResult += part;

    return wszResult;
}

std::wstring HooksPathes::GetNewConfigsFull() 
{
    std::string szMainPathFull;

    for (const auto &part : PEByte::Pathes::szMainPath)
        szMainPathFull += part;

    std::string szConfigsPrefixFull;

    for (const auto &part : PEByte::Pathes::szConfigsPrefix)
        szConfigsPrefixFull += part;

    std::wstring wszResult =
        std::wstring(szMainPathFull.begin(), szMainPathFull.end()) +
        std::wstring(szConfigsPrefixFull.begin(), szConfigsPrefixFull.end());

    return wszResult;
}

size_t HooksPathes::FindCaseInsensitive(const std::wstring &wszStr, const std::wstring &wszSubStr)
{
    std::wstring wszStrLower = wszStr;
    std::wstring wszSubStrLower = wszSubStr;

    std::transform(wszStrLower.begin(), wszStrLower.end(), wszStrLower.begin(),
                   ::towlower);
    std::transform(wszSubStrLower.begin(), wszSubStrLower.end(), wszSubStrLower.begin(),
                   ::towlower);

    size_t dwPos = wszStrLower.find(wszSubStrLower);
   
    if (dwPos != std::wstring::npos)
        return dwPos;

    return std::wstring::npos;
}

std::wstring HooksPathes::RedirectPath(const std::wstring &wzsOriginalPath) 
{
    if (wzsOriginalPath.empty())
        return wzsOriginalPath;

    static std::wstring wszOldMainFull = GetOldMainFull();
    static std::wstring wszOldConfigsFull = GetOldConfigsFull();
    static std::wstring wszNewMainFull = GetNewMainFull();
    static std::wstring wszNewConfigsFull = GetNewConfigsFull();

    size_t dwOldMainPos = FindCaseInsensitive(wzsOriginalPath, wszOldMainFull);
  
    if (dwOldMainPos == std::wstring::npos)
        return wzsOriginalPath;

    size_t dwOldConfigsPos = FindCaseInsensitive(wzsOriginalPath, wszOldConfigsFull);
    bool bIsConfigs = (dwOldConfigsPos != std::wstring::npos);

    std::wstring wszRedirected = wzsOriginalPath;

    if (bIsConfigs)
    {
        if (dwOldConfigsPos != std::wstring::npos)
        {
            wszRedirected.replace(dwOldConfigsPos, wszOldConfigsFull.length(),
                               wszNewConfigsFull);
        }
    }
    else 
    {
        if (dwOldMainPos != std::wstring::npos)
        {
            wszRedirected.replace(dwOldMainPos, wszOldMainFull.length(),
                               wszNewMainFull);
        }
    }

    return wszRedirected;
}

void HooksPathes::CreateTargetDirectory(const std::wstring &szwPath) 
{
    std::filesystem::path TargetDir = std::filesystem::path(szwPath).parent_path();

    if (!std::filesystem::exists(TargetDir)) 
    {
        std::error_code ec;
        std::filesystem::create_directories(TargetDir, ec);
    }
}