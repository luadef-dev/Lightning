#pragma once

#include "../Resources/Resources.h"

#include "Process/Process.h"
#include "Patterns/PatternSearch.h"

namespace PEAllocator
{
	namespace HelperData
	{
		inline blackbone::Process bProcess;
		inline Resources::DataInfo_t pBinary;

		inline uintptr_t BinaryBaseAddress;
	}

	blackbone::call_result_t<blackbone::ModuleDataPtr> Map();

	void Fix();
	void ReadyCS2();
	void AllocateUserData();

	bool Attach();

	void Start();
}