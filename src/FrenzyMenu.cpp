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
		case Type::kShow:
			{
				const RE::GFxValue args{ 0 };
				this->uiMovie->Invoke("_root.main.setMeterPercent", nullptr, &args, 1);
				this->uiMovie->Invoke("_root.main.show", nullptr, nullptr, 0);
			}
			return Result::kHandled;
		case Type::kForceHide:
		case Type::kHide:
			{
				const RE::GFxValue args{ *a_message.type == Type::kForceHide };
				this->uiMovie->Invoke("_root.main.hide", nullptr, &args, 1);
				FrenzyLevel->value = 0.0f;
				CurrentLevel = 0;
			}
			return Result::kHandled;
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

} // namespace NightmareNight
