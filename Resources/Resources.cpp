#include "Resources.h"

#include "../Globals.h"

HRSRC Resources::Find(eResources iID, std::string szName)
{
    HRSRC hResource = FindResourceA(Globals::hModule, reinterpret_cast<LPCSTR>(iID), szName.c_str());

    if (!hResource)
        return nullptr;

    return hResource;
}

DWORD Resources::Size(HRSRC hResource)
{
    DWORD dwSize = SizeofResource(Globals::hModule, hResource);

    if (dwSize == 0)
        return false;

    return dwSize;
}

HGLOBAL Resources::Load(HRSRC hResource)
{
    HGLOBAL hGlobal = LoadResource(Globals::hModule, hResource);

    if (!hGlobal)
        return nullptr;

    return hGlobal;
}

bool Resources::LoadResource(eResources iID, std::string szName, DataInfo_t& DataInfo) 
{
    HRSRC hResource = Find(iID, szName);

    if (!hResource)
        return false;

    DWORD dwSize = Size(hResource);

    if (dwSize == 0)
        return false;

    HGLOBAL hGlobal = Load(hResource);

    if (!hGlobal)
        return false;

    LPVOID pResourceData = LockResource(hGlobal);

    if (!pResourceData)
        return false;

    DataInfo.Buffer = new std::uint8_t[dwSize];

    if (!DataInfo.Buffer)
        return false;

    memcpy(DataInfo.Buffer, pResourceData, dwSize);
    DataInfo.dwSize = static_cast<std::size_t>(dwSize);

    return true;
}