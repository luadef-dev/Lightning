#pragma once

#include "../Resources/Resources.h"

#include <string_view>
#include <vector>
#include <cstdint>

namespace Utilities
{
	namespace FileManager
	{
		bool AddDefaultCloudConfigToFolder(std::string_view szFile,
			Resources::eResources iResource, std::string_view szNameBinary);
        bool CreateDirectoryCustom(std::string_view szPath);
        bool OpenBinary(std::string_view szPath,
                        std::vector<std::uint8_t> &VecData);
	}

#ifndef _DEBUG
	bool EnableDebugPrevilegies();
#endif

	int CreateErrorMessage(std::string_view szStr);
	int CreateInformationMessage(std::string_view szStr);

#if _DEBUG
	bool CreateConsole();

	void CreateConsoleMessage(std::string_view szStr);
#endif
}