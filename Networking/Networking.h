#pragma once

#include <string_view>

namespace Networking 
{
	bool ValidateUser(std::string_view szToken);
	
	void Start();
}