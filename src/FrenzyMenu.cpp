#include "FrenzyMenu.h"

namespace NightmareNight
{
	FrenzyMenu::FrenzyMenu()
	{
		this->inputContext = Context::kNone;
		this->depthPriority = 0;
		this->menuFlags.set(
			Flag::kAllowSaving,
			Flag::kRequiresUpdate);

		auto scaleform = RE::BSScaleformManager::GetSingleton();
		[[maybe_unused]] bool success = scaleform->LoadMovieEx(this, FILEPATH, [](RE::GFxMovieDef* a_def) -> void {
			a_def->SetState(
				RE::GFxState::StateType::kLog,
				RE::make_gptr<FlashLogger<FrenzyMenu>>().get());
		});
		assert(success);

		auto view = this->uiMovie;
		view->SetMouseCursorCount(0);
	}

	RE::UI_MESSAGE_RESULTS FrenzyMenu::ProcessMessage(RE::UIMessage& a_message)
	{
		using UEFlag = RE::ControlMap::UEFlag;
		using Type = RE::UI_MESSAGE_TYPE;
		using Result = RE::UI_MESSAGE_RESULTS;

		switch (*a_message.type) {
		// case Type::kShow:
		//	return Result::kHandled;
		// case Type::kForceHide:
		// case Type::kHide:
		// 	return Result::kHandled;
		case Type::kUpdate:
			if (FrenzyLevel->value != CurrentLevel) {
				CurrentLevel = FrenzyLevel->value;
				const RE::GFxValue args{ (CurrentLevel * 10) };
				this->uiMovie->Invoke("_root.main.updateMeterPercent", nullptr, &args, 1);
			}
			return Result::kHandled;
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}

	void FrenzyMenu::FadeIn()
	{
		if (!IsOpen())
			Show();

		SKSE::GetTaskInterface()->AddUITask([]() {
			const auto menu = RE::UI::GetSingleton()->GetMenu(NAME);
			if (!menu) {
				logger::error("Unable to retrieve Frenzy Menu");
				return;
			}
			menu->uiMovie->Invoke("_root.main.show", nullptr, nullptr, 0);
		});
	}

	void FrenzyMenu::FadeOut()
	{
		SKSE::GetTaskInterface()->AddUITask([]() {
			const auto menu = RE::UI::GetSingleton()->GetMenu(NAME);
			if (!menu) {
				logger::error("Unable to retrieve Frenzy Menu");
				return;
			}
			const auto _menu = static_cast<FrenzyMenu*>(menu.get());
			const RE::GFxValue args{ false };
			const RE::GFxValue argspct{ 0 };
			_menu->uiMovie->Invoke("_root.main.hide", nullptr, &args, 1);
			_menu->uiMovie->Invoke("_root.main.updateMeterPercent", nullptr, &argspct, 1);
			_menu->FrenzyLevel->value = 0.0f;
			_menu->CurrentLevel = 0;
		});
	}


} // namespace NightmareNight
