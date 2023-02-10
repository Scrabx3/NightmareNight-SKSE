#pragma once

#include "Singleton.h"

namespace Serialization
{
	// Primary class to handle 
	class Serialize final :
		public Singleton<Serialize>
	{
	public:
		enum : std::uint32_t
		{
			_Version = 1,

      _MenuX = 'menx',
      _MenuY = 'meny'
		};

		static void SaveCallback(SKSE::SerializationInterface* a_intfc);
		static void LoadCallback(SKSE::SerializationInterface* a_intfc);
	};	// class Serialize

	inline std::string GetTypeName(uint32_t a_type)
	{
		constexpr auto size = sizeof(uint32_t);
		std::string ret{};
		ret.resize(size);
		const char* it = reinterpret_cast<char*>(&a_type);
		for (size_t i = 0, j = size - 2; i < size - 1; i++, j--)
			ret[j] = it[i];

		return ret;
	}

} // namespace Serialization
