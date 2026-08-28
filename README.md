# Disable Menu Acceleration

A small Windows **system tray** utility that stops the **Alt** key from
grabbing focus into the window menu (and the "menu-show" behaviour that goes
with it), so you can switch the keyboard layout with Alt-based hotkeys
without the focus shooting off to some menu.

---

## Why

On Windows, pressing (or releasing) **Alt** can yank input focus into the
window's own menu / ribbon / system menu. This is especially annoying when
you switch keyboard layouts with a combination that includes Alt, e.g.
`Alt+Shift` or `Shift+Alt`:

* the menu pops up or grabs focus;
* mistype the order (`Shift+Alt` instead of `Alt+Shift`) and focus is gone;
* the very next keystroke gets eaten by the menu instead of reaching your app.

This utility hooks the keyboard, neutralises that Alt behaviour, and lets you
rebind the language switch to whichever Alt/Shift/Ctrl combination you like.

---

## Features

* **Neutralise Alt** - swallows the Alt-driven focus-grab / menu-show by
  emitting a fake Alt release (`0xEA`), so Windows never snatches the menu.
* **Custom language-switch hotkeys** - trigger "next keyboard layout" with
  your own combos instead of relying on the system default.
* **Grouped, per-hand hotkeys** - pick **Left** or **Right** Alt / Shift / Ctrl:
    * `Alt + Shift`, `Shift + Alt`
    * `Shift + Shift`, `Ctrl + Shift`, `Shift + Ctrl`
    * `Alt + Space`
* **Order-independent** - because Alt is neutralised, the combo works no
  matter the press order (`Shift+Alt` == `Alt+Shift`), focus stays put.
* **Per-hotkey enable/disable** - toggle individual combos or the whole hook
  from the tray menu.
* **Dark/light aware** - follows the system theme; no visible window, lives
  in the tray.

---

## Usage

Run the executable. A tray icon appears; right-click to open the menu:

1. Pick a hotkey group and mark the **Left/Right** variant you want.
2. Checked options are active; a group shows a tick if any sibling is enabled.
3. Use **Disable all hotkeys** / **Enable all hotkeys** to pause the hook, or **Exit** to quit.

The icon reflects the current state (active / disabled).

---

## Building

* **Windows + Visual Studio 2022 (v145)** recommended, **x64**.
* Open `DisableMenuAcceleration.slnx` (or the `.vcxproj`) and build **Release**.
* The project is **self-contained**: the only dependency is a vendored copy
  of a handful of `cst` headers (7 header-only modules), so it builds
  anywhere without extra setup.

Runtime requirement: 64-bit Windows 10/11.

---

## Dependencies

* **`cst`** (header-only) - a personal utility library. Only the needed
  modules (`FormattedText`, `UniqID` and the `winapi` wrappers for `Window`,
  `WindowTheme`, `WinHandles`, `TrayIcon`, `SingleInstanceGuard`) are vendored
  under `cst/`.

---

## Contributing

Contributions are accepted. Please submit an issue or a pull request for proposed changes or bug fixes.
