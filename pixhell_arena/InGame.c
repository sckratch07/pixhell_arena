#include "InGame.h"
#include "Players.h"

void InitInGame(WindowManager* windowManager)
{
	LoadScene("Game");
	InitPlayers();
	InitProjectiles();
}

void UpdateEventInGame(WindowManager* windowManager, sfEvent* evt)
{

}

void UpdateInGame(WindowManager* windowManager)
{
	UpdatePlayers();
	UpdateProjectiles();
}

void RenderInGame(WindowManager* windowManager)
{
	DisplayPlayers(windowManager);
	DisplayProjectiles(windowManager);
}

void UIRenderInGame(WindowManager* windowManager)
{
	DisplayUIPlayers(windowManager);
}

void DestroyInGame(WindowManager* windowManager)
{
	DestroyPlayers();
	DestroyProjectiles();
}

REGISTER_STATE(InGame)