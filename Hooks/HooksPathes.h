#pragma once

#include <filesystem>
#include <string>

#include "../PE/Bytes/PEByte.h"

namespace HooksPathes
{
	std::wstring GetOldMainFull();
	std::wstring GetNewMainFull();
	std::wstring GetOldConfigsFull();
	std::wstring GetNewConfigsFull();

	size_t FindCaseInsensitive(const std::wstring &szwStr, const std::wstring &szwSubStr);

	std::wstring RedirectPath(const std::wstring &szwOriginalPath);

	void CreateTargetDirectory(const std::wstring &szwPath);
}