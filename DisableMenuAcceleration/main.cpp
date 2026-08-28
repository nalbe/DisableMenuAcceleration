// main.cpp

// Implementation-specific headers
#include "cst/winapi/SingleInstanceGuard/SingleInstanceGuard.hpp"
#include "MyWindow.h"



// Application entry point
int WINAPI wWinMain(
	_In_ HINSTANCE     hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR         pCmdLine,
	_In_ int           nCmdShow )
{
	cst::winapi::SingleInstanceGuard exists{ MyWindow::ClassName };
	if (exists) { return 0; }

	return MyWindow{}.loop();
}



