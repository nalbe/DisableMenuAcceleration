# Disable Menu Acceleration

A small Windows **system tray** utility that disables the window
*menu-show animation* and lets you rebind the *input-language* (keyboard
layout) switch to your own Alt / Shift / Ctrl combinations.

---

## Features

* **Silence the animation** - Turns off the Windows menu-show delay/acceleration so menus pop instantly.
* **Custom language-switch hotkeys** - Trigger the system's "next keyboard layout" with Alt/Shift/Ctrl combos instead of the built-in ones.
* **Grouped, per-hand hotkeys** - Choose **Left** or **Right** Alt / Shift / Ctrl freely:
    * `Alt + Shift`, `Shift + Alt`
    * `Shift + Shift`, `Ctrl + Shift`, `Shift + Ctrl`
    * `Alt + Space`
* **Per-hotkey enable/disable** - toggle individual combos or the whole hook from the tray menu.
* **Dark/light aware** - follows the system theme (no visible window, stays in the tray).

---

## Usage

Run the executable. A tray icon appears; right-click to open the menu:

1. Pick a hotkey group and mark the **Left/Right** variant you want.
2. Checked options are active; the group shows a tick if any sibling is enabled.
3. Use **Disable all hotkeys** / **Enable all hotkeys** to pause the hook, or **Exit** to quit.

The icon reflects the current state (active / disabled).

---

## Building

* **Windows + Visual Studio 2022 (v145)** recommended, **x64**.
* Open `DisableMenuAcceleration.slnx` (or the `.vcxproj`) and build **Release**.
* The project is **self-contained**: the only dependency is a vendored
  copy of a few `cst` headers (7 header-only modules), so it builds
  anywhere without extra setup.

Runtime requirement: any 64-bit Windows 10/11.

---

## Dependencies

* **`cst`** (header-only) - a personal utility library. Only the needed
  modules (`FormattedText`, `UniqID` and the legacy `winapi` wrappers for
  `Window`, `WindowTheme`, `WinHandles`, `TrayIcon`, `SingleInstanceGuard`)
  are vendored under `cst/`.

---

## Contributing

Contributions are accepted. Please submit an issue or a pull request for proposed changes or bug fixes.
