#pragma once

namespace NightmareNight
{
	struct Coordinate
	{
		float _x = 0.70f;
		float _y = 0.82f;

		bool operator==(Coordinate& a_rhs) { return a_rhs._x == _x && a_rhs._y == _y; }
		Coordinate& operator=(Coordinate& a_rhs)
		{
			_x = a_rhs._x;
			_y = a_rhs._y;
			return *this;
		}
	};
	inline static Coordinate menucoords{};

	namespace Papyrus
	{
#define REGISTERFUNC(func, c) a_vm->RegisterFunction(#func##sv, c, func)

		using VM = RE::BSScript::IVirtualMachine;
		using StackID = RE::VMStackID;

		inline void SetX(RE::StaticFunctionTag*, float x) { menucoords._x = x; }
		inline void SetY(RE::StaticFunctionTag*, float y) { menucoords._y = y; }
		inline std::vector<float> GetCoordinates(RE::StaticFunctionTag*) { return { menucoords._x, menucoords._y }; }

		inline bool Register(VM* a_vm)
		{
			REGISTERFUNC(SetX, "NNMCM");
			REGISTERFUNC(SetY, "NNMCM");
			REGISTERFUNC(GetCoordinates, "NNMCM");

			return true;
		}
	}

  class FrenzyMenu :
    public RE::IMenu
  {
		using GRefCountBaseStatImpl::operator new;
		using GRefCountBaseStatImpl::operator delete;

	public:
		static constexpr std::string_view NAME{ "NN Frenzy Meter" };
		static constexpr std::string_view FILEPATH{ "NightmareNight" };

		FrenzyMenu();
		~FrenzyMenu() = default;
		static void Register() { RE::UI::GetSingleton()->Register(NAME, Create); }
		static RE::IMenu* Create() { return new FrenzyMenu(); }

	public:
		static void Show() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr); }
		static void Hide() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr); }
		static void ForceHide() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kForceHide, nullptr); }
		static bool IsOpen() { return RE::UI::GetSingleton()->IsMenuOpen(NAME); }

		static void FadeIn();
		static void FadeOut();

	protected:
		// IMenu
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

  private:
	  RE::TESGlobal* const FrenzyLevel{ RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x87E, "NightmareNight.esp"sv) };
	  float CurrentLevel{ 0.0f };

	  Coordinate coords{ menucoords };
  };

	template <typename T>
	class FlashLogger : public RE::GFxLog
	{
	public:
		void LogMessageVarg(LogMessageType, const char* str, std::va_list a_argList) override
		{
			std::string msg(str ? str : "");
			while (!msg.empty() && msg.back() == '\n')
				msg.pop_back();

			auto length = std::vsnprintf(0, 0, msg.c_str(), a_argList) + 1;
			char* buffer = (char*)malloc(sizeof(*buffer) * length);
			if (!buffer)
				return;
			std::vsnprintf(buffer, length, msg.c_str(), a_argList);

			logger::info("{} -> {}", T::NAME, buffer);
			free(buffer);
		}
	};
} // namespace NightmareNight
