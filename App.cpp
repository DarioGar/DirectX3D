#include "App.h"
#include <iostream>

float x = 0;
char carc;
App::App()
	:
	wnd(800, 600, "DirectX3D")
{}

int App::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	
	using namespace std;
	const float c = sin( timer.Peek() ) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	carc = wnd.kbd.ReadChar();
	if ( carc == 'w' && x <= 7) {
		x += 0.1;
	}
	if (carc == 's' && x >= 0) {
		x -= 0.1;
	}

	wnd.Gfx().DrawCube(timer.Peek(),x);
	wnd.Gfx().EndFrame();
	
}
