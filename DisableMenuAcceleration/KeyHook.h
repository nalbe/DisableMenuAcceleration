// KeyHook.h
#pragma once

// Standard library headers
#include <stdexcept>

// Windows system headers
#include <Windows.h>



class KeyHook
{
private:
	static inline HHOOK    s_hHook     {};
	static inline KeyHook* s_pInstance {};

public:
	enum class HotKey
	{
		none,
		LeftAlt_LeftShift,
		RightAlt_RightShift,
		LeftShift_LeftAlt,
		RightShift_RightAlt,
		LeftShift_RightShift,
		RightShift_LeftShift,
		LeftCtrl_LeftShift,
		RightCtrl_RightShift,
		LeftShift_LeftCtrl,
		RightShift_RightCtrl,
		LeftAlt_Space,
		RightAlt_Space,
		size
	};

private:
	bool m_enabled {};
	bool m_hotkey[static_cast<uint16_t>(HotKey::size)] {};

public:
	bool install();
	void uninstall();

	void enable(HotKey)                 noexcept;
	void enable()                       noexcept;
	void disable(HotKey)                noexcept;
	void disable()                      noexcept;
	bool enabled(HotKey)                const noexcept;
	bool enabled()                      const noexcept;
	bool toggle(HotKey)                 noexcept;
	bool toggle()                       noexcept;

public:
	KeyHook()                           = default;
	KeyHook(const KeyHook&)             = delete;
	KeyHook& operator=(const KeyHook&)  = delete;
	KeyHook(KeyHook&&)                  = default;
	KeyHook& operator=(KeyHook&&)       = default;

private:
	static LRESULT CALLBACK s_HookProc(int, WPARAM, LPARAM);
	LRESULT hookProc(int, WPARAM, LPARAM);

private:
	void send_key(WORD, bool);
	void send_fake();
	void switch_to_next_keyboard_layout();

};  // class KeyHook



