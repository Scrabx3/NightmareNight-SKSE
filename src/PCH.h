#pragma once

#pragma warning(push)
#pragma warning(disable : 4200)
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#pragma warning(pop)

#include <unordered_map>
#include <atomic>

#pragma warning(push)
#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

namespace logger = SKSE::log;
namespace fs = std::filesystem;
using namespace std::literals;

namespace stl
{
	using namespace SKSE::stl;

	constexpr std::uint32_t version_pack(REL::Version a_version) noexcept
	{
		return static_cast<std::uint32_t>(
			(a_version[0] & 0x0FF) << 24u |
			(a_version[1] & 0x0FF) << 16u |
			(a_version[2] & 0xFFF) << 4u |
			(a_version[3] & 0x00F) << 0u);
	}

	template <class F, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[0] };
		T::func = vtbl.write_vfunc(T::size, T::thunk);
	}
}

#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"
