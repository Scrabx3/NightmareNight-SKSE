#include "ActorValueMeter.h"
#include "FrenzyMenu.h"
#include "Serialize.h"

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	const auto InitLogger = [&plugin]() -> bool {
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path)
			return false;
		*path /= std::format("{}.log", plugin->GetName());
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif
		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
#ifndef NDEBUG
		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);
#else
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
#endif
		spdlog::set_default_logger(std::move(log));
#ifndef NDEBUG
		spdlog::set_pattern("%s(%#): [%T] [%^%l%$] %v"s);
#else
		spdlog::set_pattern("[%T] [%^%l%$] %v"s);
#endif

		logger::info("{} v{}", plugin->GetName(), plugin->GetVersion());
		return true;
	};

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible");
		return false;
	} else if (!InitLogger()) {
		logger::critical("Failed to initialize logger");
		return false;
	}

	SKSE::Init(a_skse);

	NightmareNight::Install();
	if (REL::Module::IsVR()) {
		logger::info("VR version detected. Skipping Menu install.");
	} else {
		NightmareNight::FrenzyMenu::Register();
		logger::info("Frenzy Menu registered");
	}

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(NightmareNight::Papyrus::Register);

	const auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID('NiNi');
	serialization->SetSaveCallback(Serialization::Serialize::SaveCallback);
	serialization->SetLoadCallback(Serialization::Serialize::LoadCallback);
	// serialization->SetRevertCallback(Serialization::Serialize::RevertCallback);
	// serialization->SetFormDeleteCallback(Serialization::Serialize::FormDeleteCallback);

	logger::info("Initialization complete");

	return true;
}
