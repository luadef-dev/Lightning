#pragma once

#include <Windows.h>
#include <string>
#include <format>
#include <fstream>
#include <ranges>
#include <algorithm>
#include <filesystem>
#include <vector>

#include "Library/base64.hpp"
#include "Library/json.hpp"
#include "Library/httplib.h"
#include "Library/xorstr.hpp"

namespace Globals
{
	inline std::string szMessagePrefix = xorstr_("");

	namespace Pathes
	{
		inline std::string szConfigsPath = xorstr_("");
		inline std::string szCrashExceptionFilePath = xorstr_("");
		inline std::string szCrashTopLevelExceptionFilePath = xorstr_("");
		inline std::string szUserFilePath = xorstr_("");
        inline std::string szUserAvatarPath = xorstr_("");
	}

	namespace UserData
	{
        inline HGLOBAL hGlobalAvatar = nullptr; 
        inline HRSRC hResInfoAvatar = nullptr;

        inline std::string szUserName = szMessagePrefix = xorstr_("");
		inline std::string szSubLeft = xorstr_("Never");

		inline std::vector<std::uint8_t> VecAvatar = {};
	}

	namespace Networking
	{
        inline httplib::Headers Headers;
	}

	inline HMODULE hModuleNavSystem = nullptr;
	inline HMODULE hModule = nullptr;
}