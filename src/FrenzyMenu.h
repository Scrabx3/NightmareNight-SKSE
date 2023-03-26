#pragma once

namespace NightmareNight
{
	struct Coordinates
	{
		Coordinates(float x, float y) :
			_x(x), _y(y) {}
		float _x;
		float _y;

		bool operator==(Coordinates& a_rhs) { return a_rhs._x == _x && a_rhs._y == _y; }
		Coordinates& operator=(Coordinates& a_rhs)
		{
			_x = a_rhs._x;
			_y = a_rhs._y;
			return *this;
		}
	};

  class FrenzyMenu :
    public RE::IMenu
  {
		using GRefCountBaseStatImpl::operator new;
		using GRefCountBaseStatImpl::operator delete;

	public:
		inline static Coordinates MenuCoordinates{ 0.70f, 0.82f };

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
		Coordinates CurrentCoordinates{ MenuCoordinates };

		RE::TESGlobal* const FrenzyLevel{ RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x87E, "NightmareNight.esp"sv) };
		float CurrentLevel{ 0.0f };
  };

  namespace Papyrus
  {
#define REGISTERFUNC(func, c) a_vm->RegisterFunction(#func##sv, c, func)

	  using VM = RE::BSScript::IVirtualMachine;
	  using StackID = RE::VMStackID;

	  inline void SetX(RE::StaticFunctionTag*, float x) { FrenzyMenu::MenuCoordinates._x = x; }
	  inline void SetY(RE::StaticFunctionTag*, float y) { FrenzyMenu::MenuCoordinates._y = y; }
	  inline std::vector<float> GetCoordinates(RE::StaticFunctionTag*) { return { FrenzyMenu::MenuCoordinates._x, FrenzyMenu::MenuCoordinates._y }; }

	  inline bool Register(VM* a_vm)
	  {
			REGISTERFUNC(SetX, "NNMCM");
			REGISTERFUNC(SetY, "NNMCM");
			REGISTERFUNC(GetCoordinates, "NNMCM");

			return true;
	  }
  }

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
