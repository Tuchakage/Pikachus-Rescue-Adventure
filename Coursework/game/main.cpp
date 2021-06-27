#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	app.OpenWindow(1280, 720, "Pikachus Rescue Adventure!");
	//app.OpenFullScreen(1280, 720, 24);
	app.SetClearColor(CColor::Green(128));
	app.Run(&game);
	return(0);
}
