#pragma once

#include <Windows.h>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Image/resource.h"

namespace Resources
{
    enum eResources : int {
        IDR_BINARY = IDR_BINARY1,
        IDR_CFG_BY_SHIYATO = IDR_CFG_BY_SHIYATO1
    };

    struct DataInfo_t
    {
        std::size_t dwSize;
        std::uint8_t* Buffer;
    };

    HRSRC Find(eResources iID, std::string szName);
    DWORD Size(HRSRC hResource);
    HGLOBAL Load(HRSRC hResource);

    bool LoadResource(eResources iID, std::string szName, DataInfo_t &DataInfo);
}