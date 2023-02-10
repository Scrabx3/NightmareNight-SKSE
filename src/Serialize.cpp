#include "Serialize.h"

#include "FrenzyMenu.h"

namespace Serialization
{
	void Serialize::SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		if (!a_intfc->OpenRecord(_MenuX, _Version))
			logger::error("Failed to open record <_MenuX>"sv);
		else if (!a_intfc->WriteRecordData(NightmareNight::menucoords._x))
			logger::error("Failed to serialize record <_MenuX>"sv);

		if (!a_intfc->OpenRecord(_MenuY, _Version))
			logger::error("Failed to open record <_MenuY>"sv);
		else if (!a_intfc->WriteRecordData(NightmareNight::menucoords._y))
			logger::error("Failed to serialize record <_MenuY>"sv);
	}

	void Serialize::LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		uint32_t type;
		uint32_t version;
		uint32_t length;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (version != _Version) {
				logger::warn("Invalid Version for loaded Data of Type = {}. Expected = {}; Got = {}", GetTypeName(type), _Version, version);
				continue;
			}
			switch (type) {
			case _MenuX:
				logger::info("Loading record <_MenuX>"sv);
				if (float x; a_intfc->ReadRecordData(x))
					NightmareNight::menucoords._x = x;
				else
					logger::error("Failed to read Record Data <_MenuX>"sv);
				break;
			case _MenuY:
				logger::info("Loading record <_MenuY>"sv);
				if (float y; a_intfc->ReadRecordData(y))
					NightmareNight::menucoords._y = y;
          else
						logger::error("Failed to read Record Data <_MenuY>"sv);
				break;
			default:
				logger::warn("Unrecognized type: {}", GetTypeName(type));
				break;
			}
		}
	}

}  // namespace Serialization
