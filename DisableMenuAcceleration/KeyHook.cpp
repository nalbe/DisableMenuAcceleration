// KeyHook.cpp

// Implementation-specific headers
#include "KeyHook.h"

// bit ops
#define BitRead(src, n)         (((src) >> (n)) & 0x01)
#define BitSet(src, n)          ((src) |= (1UL << (n)))
#define BitClear(src, n)        ((src) &= ~(1UL << (n)))
#define BitToggle(src, n)       ((src) ^= (1UL << (n)))
#define BitWrite(src, n, value) ((value) ? BitSet(src, n) : BitClear(src, n))



// install hook
bool KeyHook::install()
{
	if (s_hHook) { return true; }

	HMODULE hMod = ::GetModuleHandleW(nullptr);
	s_hHook      = ::SetWindowsHookExW(WH_KEYBOARD_LL, s_HookProc, hMod, 0);
	s_pInstance  = this;
	return s_hHook != nullptr;
}

// uninstall hook
void KeyHook::uninstall()
{
	if (s_hHook) {
		::UnhookWindowsHookEx(s_hHook);
		s_hHook = nullptr;
	}
}


// enable a hotkey
void KeyHook::enable(HotKey hk) noexcept
{
	m_hotkey[static_cast<uint16_t>(hk)] = true;
}

// enable keyboard hook
void KeyHook::enable() noexcept
{
	m_enabled = true;
}

// disable a hotkey
void KeyHook::disable(HotKey hk) noexcept
{
	m_hotkey[static_cast<uint16_t>(hk)] = false;
}

// disable keyboard hook
void KeyHook::disable() noexcept
{
	m_enabled = false;
}

// check if a hotkey is enabled
bool KeyHook::enabled(HotKey hk) const noexcept
{
	return m_hotkey[static_cast<uint16_t>(hk)];
}

// check if keyboard hook is enabled
bool KeyHook::enabled() const noexcept
{
	return m_enabled;
}

// toggle a hotkey
bool KeyHook::toggle(HotKey hk) noexcept
{
	return m_hotkey[static_cast<uint16_t>(hk)] = !m_hotkey[static_cast<uint16_t>(hk)];
}

// toggle keyboard hook
bool KeyHook::toggle() noexcept
{
	return m_enabled = !m_enabled;
}


// send a fake scancode (Taran's trick)
void KeyHook::send_fake()
{
	send_key(0xEA, false);
}

// send a scancode
void KeyHook::send_key(WORD scancode, bool down)
{
	INPUT input{};
	input.type       = INPUT_KEYBOARD;
	input.ki.wVk     = 0;
	input.ki.wScan   = scancode;
	input.ki.dwFlags = KEYEVENTF_SCANCODE | (down ? 0 : KEYEVENTF_KEYUP);
	::SendInput(1, &input, sizeof(INPUT));
}

// switch to next keyboard layout for the foreground window's thread
void KeyHook::switch_to_next_keyboard_layout()
{
	HWND hwnd = ::GetForegroundWindow();
	if (!hwnd) { return; }

	DWORD threadId = ::GetWindowThreadProcessId(hwnd, nullptr);
	HKL current    = ::GetKeyboardLayout(threadId);
	UINT count     = ::GetKeyboardLayoutList(0, nullptr);

	if (count < 2) { return; }
	HKL* layouts = new HKL[count];
	::GetKeyboardLayoutList(count, layouts);

	int idx = -1;
	for (UINT i{}; i < count; ++i) {
		if (layouts[i] == current) {
			idx = static_cast<int>(i);
			break;
		}
	}
	if (idx == -1) {
		delete[] layouts;
		return;
	}
	HKL next = layouts[(idx + 1) % count];
	delete[] layouts;

	::PostMessageW(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(next));
}


// static hook proc
LRESULT CALLBACK KeyHook::s_HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return s_pInstance
		? s_pInstance->hookProc(nCode, wParam, lParam)
		: ::CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// hook proc
LRESULT KeyHook::hookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!enabled() or nCode < 0 or !s_hHook) {
		return ::CallNextHookEx(s_hHook, nCode, wParam, lParam);
	}

	auto p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

	// ignore injected events
	if (p->flags & LLKHF_INJECTED) {
		return ::CallNextHookEx(s_hHook, nCode, wParam, lParam);
	}

	enum : uint16_t {
		LeftAlt,   RightAlt,
		LeftShift, RightShift,
		LeftCtrl,  RightCtrl,
		LeftWin,   RightWin,
		Space
	};
	static uint16_t held {};

	if (p->vkCode == VK_LMENU) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { BitSet(held, LeftAlt); }
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			BitClear(held, LeftAlt);
			if (BitRead(held, LeftShift) and enabled(HotKey::LeftShift_LeftAlt)) {
				switch_to_next_keyboard_layout();
			}
			send_fake();
		}
	}
	else if (p->vkCode == VK_RMENU) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { BitSet(held, RightAlt); }
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			BitClear(held, RightAlt);
			if (BitRead(held, RightShift) and enabled(HotKey::RightShift_RightAlt)) {
				switch_to_next_keyboard_layout();
			}
			send_fake();
		}
	}
	else if (p->vkCode == VK_LSHIFT) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { BitSet(held, LeftShift); }
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			BitClear(held, LeftShift);
			if (BitRead(held, LeftAlt) and enabled(HotKey::LeftAlt_LeftShift)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
			else if (BitRead(held, LeftCtrl) and enabled(HotKey::LeftCtrl_LeftShift)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
			else if (BitRead(held, RightShift) and enabled(HotKey::RightShift_LeftShift)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
		}
	}
	else if (p->vkCode == VK_RSHIFT) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { BitSet(held, RightShift); }
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			BitClear(held, RightShift);
			if (BitRead(held, RightAlt) and enabled(HotKey::RightAlt_RightShift)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
			else if (BitRead(held, RightCtrl) and enabled(HotKey::RightCtrl_RightShift)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
			else if (BitRead(held, LeftShift) and enabled(HotKey::LeftShift_RightShift)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
		}
	}
	else if (p->vkCode == VK_LCONTROL) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { BitSet(held, LeftCtrl); }
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			BitClear(held, LeftCtrl);
			if (BitRead(held, LeftShift) and enabled(HotKey::LeftShift_LeftCtrl)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
		}
	}
	else if (p->vkCode == VK_RCONTROL) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { BitSet(held, RightCtrl); }
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			BitClear(held, RightCtrl);
			if (BitRead(held, RightShift) and enabled(HotKey::RightShift_RightCtrl)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
		}
	}
	else if (p->vkCode == VK_SPACE) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			if (BitRead(held, LeftAlt) and enabled(HotKey::LeftAlt_Space)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
			else if (BitRead(held, RightAlt) and enabled(HotKey::RightAlt_Space)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
			BitSet(held, Space);
		}
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			BitClear(held, Space);
			if (BitRead(held, LeftAlt) and enabled(HotKey::LeftAlt_Space)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
			else if (BitRead(held, RightAlt) and enabled(HotKey::RightAlt_Space)) {
				switch_to_next_keyboard_layout();
				send_fake();
			}
		}
	}

	return ::CallNextHookEx(s_hHook, nCode, wParam, lParam);
}



