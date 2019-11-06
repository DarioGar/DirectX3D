#include <Windows.h>
#include <string>
#include "Window.h"
#include "App.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	try {
		return App{}.Go();
	}
	catch (const Direct3DException& e)
	{
		MessageBox(nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(),"Excepción estandar", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details", "Unknown", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}