#include "Frenzy.h"

namespace NightmareNight
{
	enum class FrenzyStatus
	{
		Inactive,
		Transformed,
		Frenzied
	};

	struct Frenzy :
		Singleton<Frenzy>
	{
	private:
		static inline FrenzyStatus status;
		static inline std::mutex _m;

		static std::optional<float> GetRemainingDuration()
		{
			static const auto BloodFrenzy = RE::TESDataHandler::GetSingleton()->LookupForm<RE::EffectSetting>(0x9EC, "NightmareNight.esp"sv);
			const auto player = RE::PlayerCharacter::GetSingleton();
			const auto list = player->GetActiveEffectList();
			if (list) {
				for (auto& effect : *list) {
					if (const auto base = effect->GetBaseObject(); !base || base != BloodFrenzy)
						continue;
					else if (effect->flags.any(RE::ActiveEffect::Flag::kDispelled, RE::ActiveEffect::Flag::kInactive))
						break;
					return (effect->duration - effect->elapsedSeconds) / effect->duration;
				}
			}
			return std::nullopt;
		}

	public:
		static void thunk(RE::HUDChargeMeter* a_meter)
		{
			switch (status) {
			case FrenzyStatus::Inactive:
				return func(a_meter);
			case FrenzyStatus::Transformed:
				a_meter->root.Invoke("FadeOutChargeMeters");
				break;
			case FrenzyStatus::Frenzied:
				{
					std::scoped_lock lock(_m);
					auto percent = GetRemainingDuration();
					if (!percent && status == FrenzyStatus::Frenzied) {
						status = FrenzyStatus::Transformed;
						break;
					}
					std::array<RE::GFxValue, 4> array{ *percent, true, true, true };
					a_meter->root.Invoke("SetChargeMeterPercent", nullptr, array);
				}
				break;
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t size = 0x1;

		static void SetStatus(RE::StaticFunctionTag*, int32_t a_status)
		{
			std::scoped_lock lock(_m);
			status = FrenzyStatus(a_status);
		}
	};

	inline bool Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("SetStatus"sv, "NNFrenzyController", Frenzy::SetStatus);
		stl::write_vfunc<RE::HUDChargeMeter, Frenzy>();
		return true;
	}
}  // namespace NightmareNight

// static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
// {
// 	switch (message->type) {
// 	case SKSE::MessagingInterface::kSaveGame:
// 		break;
// 	case SKSE::MessagingInterface::kDataLoaded:
// 		break;
// 	case SKSE::MessagingInterface::kNewGame:
// 	case SKSE::MessagingInterface::kPostLoadGame:
// 		break;
// 	}
// }

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("Scrab Joséline"sv);
	v.UsesAddressLibrary(true);
	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION.pack();
	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	const auto InitLogger = []() -> bool {
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path)
			return false;
		*path /= fmt::format(FMT_STRING("{}.log"), Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif
		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);

		logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
		return true;
	};
	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	} else if (!InitLogger()) {
		return false;
	}

	SKSE::Init(a_skse);
	logger::info("{} loaded"sv, Plugin::NAME);

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(NightmareNight::Register);

	logger::info("Initialization complete");

	return true;
}
