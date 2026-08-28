// MyWindow.h
#pragma once

// Implementation-specific headers
#include "cst/winapi/Window/Window.hpp"
#include "cst/winapi/WindowTheme/WindowTheme.hpp"
#include "cst/winapi/WinHandles/WinHandles.hpp"
#include "cst/winapi/TrayIcon/TrayIcon.hpp"
#include "cst/UniqID/UniqID.hpp"
#include "KeyHook.h"
#include "resource.h"



class MyWindow :
	public cst::winapi::WindowThemeMixin<MyWindow>,
	public cst::winapi::Window,
	public cst::winapi::TrayIcon
{
public:
	static constexpr LPCWSTR ClassName  = L"DisableMenuAccelerationClass";
	static constexpr LPCWSTR ClassTitle = L"Disable Menu Acceleration";

private:
	KeyHook keyHook;
	const cst::UniqID UWM_Enabled {};
	const cst::UniqID UWM_Exit    {};
	const cst::UniqID UWM_hotkey[static_cast<uint16_t>(KeyHook::HotKey::size)] {};

public:
	MyWindow() :

		Window( Window::Config{}
			.withClassName         ( ClassName  )
			.withClassTitle        ( ClassTitle )
			.withMessageHandler    ( [this](auto... args) { return WndProc(args...); })
			.withIcon              ( ::LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDI_ICON1)), false)
			.withImmediateCreation ()
			(/* end of Window config */)
		),

		TrayIcon( TrayIcon::Config{}
			.withID                ( cst::UniqID{}      )
			.withCallbackMessage   ( cst::UniqID{}      )
			.withTooltip           ( ClassTitle         )
			.withVersion           ( NOTIFYICON_VERSION )
			.withFlags             ( NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP | NIF_ICON )
			.withIcons             ( new HICON[] {
						::LoadIconW(m_hInstance, MAKEINTRESOURCE(IDI_ICON2)),  // disabled
						::LoadIconW(m_hInstance, MAKEINTRESOURCE(IDI_ICON1))
					}, 2, false )
			.withHandle            ( handle() )
			.withMenu              (
				[&]() -> HMENU {
					HMENU menu = CreatePopupMenu();
					using HK = KeyHook::HotKey;

					auto append = [menu](UINT id, LPCWSTR text, bool checked, bool grayed = false) {
						UINT flags = MF_STRING | (checked ? MF_CHECKED : 0) | (grayed ? (MF_GRAYED | MF_DISABLED) : 0);
						::AppendMenuW(menu, flags, id, text);
					};
					auto label = [&](LPCWSTR text) {
						append(0, text, false, true);
					};
					auto separator = [&]() {
						::AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
					};

					// build a submenu holding the Left/Right variant of one hotkey group
					auto addGroup = [&](LPCWSTR title, HK hkLeft, HK hkRight, LPCWSTR textLeft, LPCWSTR textRight) {
						HMENU sub = CreatePopupMenu();
						bool any = false;
						auto mk = [&](HK hk, LPCWSTR text) {
							bool en = keyHook.enabled(hk);
							any = any or en;
							UINT flags = MF_STRING;
							if (en)                 flags |= MF_CHECKED | 0x00000200u;  // MFT_RADIOCHECK
							if (!keyHook.enabled()) flags |= MF_GRAYED | MF_DISABLED;
							::AppendMenuW(sub, flags, UWM_hotkey[static_cast<UINT>(hk)], text);
						};
						mk(hkLeft,  textLeft );
						mk(hkRight, textRight);
						UINT flags = MF_POPUP | (any && keyHook.enabled() ? MF_CHECKED : 0);
						if (!keyHook.enabled()) flags |= MF_GRAYED | MF_DISABLED;
						::AppendMenuW(menu, flags, reinterpret_cast<UINT_PTR>(sub), title);
					};

					label(L"-- Additional HotKeys --");
					separator();
					addGroup(L"Alt + Shift",        HK::LeftAlt_LeftShift,    HK::RightAlt_RightShift,
						L"Left  Alt + Left  Shift", L"Right Alt + Right Shift");
					addGroup(L"Shift + Alt",        HK::LeftShift_LeftAlt,    HK::RightShift_RightAlt,
						L"Left  Shift + Left  Alt", L"Right Shift + Right Alt");
					addGroup(L"Shift + Shift",      HK::LeftShift_RightShift, HK::RightShift_LeftShift,
						L"Left  Shift + Right Shift", L"Right Shift + Left  Shift");
					addGroup(L"Ctrl + Shift",       HK::LeftCtrl_LeftShift,   HK::RightCtrl_RightShift,
						L"Left  Ctrl + Left  Shift", L"Right Ctrl + Right Shift");
					addGroup(L"Shift + Ctrl",       HK::LeftShift_LeftCtrl,   HK::RightShift_RightCtrl,
						L"Left  Shift + Left  Ctrl", L"Right Shift + Right Ctrl");
					addGroup(L"Alt + Space",        HK::LeftAlt_Space,        HK::RightAlt_Space,
						L"Left  Alt + Space",       L"Right Alt + Space");
					separator();
					append(UWM_Enabled, keyHook.enabled() ? L"Disable all" : L"Enable all", false);
					separator();
					append(UWM_Exit, L"Exit", false);

					return menu;
				})
			.withImmediateCreation()
			(/* end of TrayIcon config */)
		)
	{
		keyHook.enable();
		keyHook.enable(KeyHook::HotKey::LeftShift_LeftAlt);

		TrayIcon::icon
			.select(keyHook.enabled() ? 1 : 0)
			.update();
	}


	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		if (TrayIcon::hasMessage(uMsg)) {
			if (TrayIcon::hasID(wParam)) {
				if (lParam == WM_RBUTTONUP or lParam == WM_CONTEXTMENU) {
					if (!TrayIcon::menu.show()) { return 0; }
				}
				return 0;
			}
		}


		switch (uMsg)
		{

		case WM_CREATE:
		{
			if (!keyHook.install()) { return 1; }  // start keyboard hook
			follow_system_theme();

			return 0;
		}

		case WM_COMMAND:
		{
			WORD id = LOWORD(wParam);

			if (id == UWM_Enabled) {
				keyHook.toggle();
				TrayIcon::icon
					.select(keyHook.enabled() ? 1 : 0)
					.update();
				return 0;
			}
			for (size_t i{}; i < std::size(UWM_hotkey); ++i) {
				if (id == UWM_hotkey[i]) {
					keyHook.toggle(static_cast<KeyHook::HotKey>(i));
					return 0;
				}
			}
			if (id == UWM_Exit) {
				destroy();
				return 0;
			}
			break;
		}

		case WM_DESTROY:
		{
			keyHook.uninstall();  // stop keyboard hook
			::PostQuitMessage(0);
			return 0;
		}
		default: break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

};  // class MyWindow



