#pragma once

#include "FrenzyMenu.h"

namespace NightmareNight
{
	struct Update
	{
		static void thunk(RE::ActorValueMeter& meter)
		{
			if (meter.actorValue != RE::ActorValue::kMagicka)
				return func(meter);

			if (IsTransformed()) {
				const auto pct = GetFrenzyPct();
				if (!pct) {
					// Set Magicka Meter to 0 on first tick after transformation
					if (!_transformed) {
						_transformed = true;
						SetMeterPct(meter, 0);
  					FrenzyMenu::Show();
					} else if (_frenzied) {
						_frenzied = false;
						SetMeterPct(meter, 0);
						meter.root.Invoke(meter.setBlinkingName.c_str());
  					FrenzyMenu::FadeOut();
					}
					return;
				} else if (auto alpha = meter.view->GetVariableDouble(MagicaAlpha); alpha < 100) {
					meter.view->SetVariableDouble(MagicaAlpha, 100);
				}
				if (!_frenzied) {
					_frenzied = true;
  				FrenzyMenu::FadeIn();
				}
				SetMeterPct(meter, *pct);
				return;
			} else if (_transformed) {
				_frenzied = false;
				_transformed = false;
  			FrenzyMenu::ForceHide();
			}
			return func(meter);
		}

		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t size = 0x1;

	private:
		static constexpr const char* MagicaAlpha{ "_root.HUDMovieBaseInstance.Magica._alpha" };
		static inline bool _transformed{ false };
		static inline bool _frenzied{ false };

		static void SetMeterPct(RE::ActorValueMeter& meter, double percent)
		{
			std::array<RE::GFxValue, 2> args{ percent, false };
			meter.root.Invoke(meter.setPctName.c_str(), nullptr, args);
		}

		static inline bool IsTransformed()
		{
			static const auto werebeast = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSKeyword>(0x8F2, "NightmareNight.esp"sv);
			static const auto requiem = RE::TESDataHandler::GetSingleton()->LookupForm<RE::EffectSetting>(0x909, "NightmareNight.esp"sv);
			const auto player = RE::PlayerCharacter::GetSingleton();
			return player->HasKeyword(werebeast) || player->AsMagicTarget()->HasMagicEffect(requiem);
		}

		static std::optional<float> GetFrenzyPct()
		{
			static const auto bloodfrenzy = RE::TESDataHandler::GetSingleton()->LookupForm<RE::EffectSetting>(0x897, "NightmareNight.esp"sv);
			const auto player = RE::PlayerCharacter::GetSingleton();
			const auto effects = player->AsMagicTarget()->GetActiveEffectList();
			if (!effects)
				return std::nullopt;
			for (auto& e : *effects) {
				if (!e || e->GetBaseObject() != bloodfrenzy)
					continue;
				else if (e->flags.any(RE::ActiveEffect::Flag::kDispelled, RE::ActiveEffect::Flag::kInactive))
					continue;
				const float dur = e->duration - e->elapsedSeconds;
				return (dur / e->duration) * 100.0f;
			}
			return std::nullopt;
		}
	};

	inline void Install()
	{
		stl::write_vfunc<RE::ActorValueMeter, Update>();
	}
}
