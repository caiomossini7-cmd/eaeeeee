#pragma once

#include <Windows.h>
#include "../../FivemSDK/Fivem.hpp"


namespace Cheat
{
	namespace ResourceManager
	{
		enum eResourceState : uint32_t
		{
			Uninitialized,
			Stopped,
			Starting,
			Started,
			Stopping
		};

		struct Resources_t 
		{
			uintptr_t Pointer = 0;
			std::string Path;
			eResourceState State = eResourceState::Uninitialized;
		};

		inline std::vector<Resources_t> vResources;

		class cResourceList
		{
			void Stop(uint64_t ResourcePtr)
			{

			}
		};

		inline cResourceList g_ResourceList;
	}
}