#include "Players.h"
#include "UI.h"

UIObjectManager* players_ui_manager;
Animation* player_animation;

sfRectangleShape* player_shape[PLAYER_NUMBER];
sfCircleShape* direction_circle[PLAYER_NUMBER];

sfVector2f GetPlayerPosition(int player_id)
{
	if (player_id < 0 || player_id >= PLAYER_NUMBER)
	{
		return sfVector2f_Create(0.f, 0.f);
	}
	return players[player_id].position;
}

void InitPlayers(void)
{
	//player_animation = CreateAnimationFromFile("", GetTexture(""));
	players_ui_manager = CreateUIObjectManager();

	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		player_shape[i] = sfRectangleShape_create();
		sfRectangleShape_setSize(player_shape[i], sfVector2f_Create(50.f, 50.f));
		sfRectangleShape_setFillColor(player_shape[i], (sfColor) { 255, 0, 0, 255 });
		sfRectangleShape_setOrigin(player_shape[i], sfVector2f_Create(25.f, 25.f));

		players[i].position = sfVector2f_Create(935.f + i * 50.f, 540.f);
		sfRectangleShape_setPosition(player_shape[i], players[i].position);

		direction_circle[i] = sfCircleShape_create();
		sfCircleShape_setRadius(direction_circle[i], 10.f);
		sfCircleShape_setFillColor(direction_circle[i], (sfColor) { 0, 255, 0, 255 });
		sfCircleShape_setOrigin(direction_circle[i], sfVector2f_Create(10.f, 10.f));

		players[i].shoot_delay = 0.2f;
		players[i].attack_delay = 0.35f;
		players[i].dodge_delay = 1.5f;
	}
}

void UpdatePlayers(void)
{
	sfVector2f left_joystick;
	sfVector2f right_joystick;
	float left_trigger;
	float right_trigger;

	for (int i = 0; i < PLAYER_NUMBER; i++)
	{
		if (players[i].is_dodging)
		{
			players[i].dodge_timer += DeltaTime;
			if (players[i].dodge_timer > 0.5f)
			{
				players[i].is_dodging = sfFalse;
				players[i].dodge_timer = 0.f;
			}
			else
			{
				players[i].position = AddVector2f(players[i].position, MultiplyVector2f(players[i].direction, 0.65f));
				sfRectangleShape_setPosition(player_shape[i], players[i].position);
			}
		}
		else
		{
			players[i].direction = sfVector2f_Create(0.f, 0.f);

			getSticksPosition(i, &left_joystick, &right_joystick);
			getTriggers(i, &left_trigger, &right_trigger);

			left_joystick.y = -left_joystick.y;
			right_joystick.y = -right_joystick.y;
			
			players[i].direction = NormalizeVector2f(left_joystick);
			players[i].position = AddVector2f(players[i].position, MultiplyVector2f(players[i].direction, 0.25f));
			sfRectangleShape_setPosition(player_shape[i], players[i].position);

			if (right_joystick.x != 0.f || right_joystick.y != 0.f)
			{
				float angle = atan2f(right_joystick.y, right_joystick.x);
				sfCircleShape_setPosition(direction_circle[i], sfVector2f_Create(players[i].position.x + cosf(angle) * 57.5f, players[i].position.y + sinf(angle) * 57.5f));
			}
			else
			{
				sfCircleShape_setPosition(direction_circle[i], players[i].position);
			}

			players[i].shoot_timer += DeltaTime;
			if (players[i].shoot_delay
	> 0.2f && right_trigger >= 0.8f)
			{
				if (right_joystick.x != 0.f || right_joystick.y != 0.f)
				{
					CreateProjectile(players[i].position, NormalizeVector2f(right_joystick), PLAYER | NORMAL_BULLET, 500.f, 1.f);
				}
				else
				{
					CreateProjectile(players[i].position, players[i].direction, PLAYER | NORMAL_BULLET, 500.f, 1.f);
				}
				players[i].shoot_timer = 0.f;
			}

			players[i].attack_timer += DeltaTime;
			if (players[i].attack_delay > 0.2f && left_trigger >= 0.8f)
			{
				printf("Player %d attack\n", i);
				players[i].attack_timer = 0.f;
			}

			players[i].dodge_timer += DeltaTime;
			if (players[i].dodge_delay > 1.5f && isButtonPressed(i, B))
			{
				players[i].is_dodging = sfTrue;
				players[i].dodge_timer = 0.f;
			}
		}
	}

	players_ui_manager->update(players_ui_manager, NULL);
}

void DisplayPlayers(WindowManager* window)
{
	window->DrawCircleShape(window, direction_circle[0], NULL);
	window->DrawCircleShape(window, direction_circle[1], NULL);
	window->DrawRectangleShape(window, player_shape[0], NULL);
	window->DrawRectangleShape(window, player_shape[1], NULL);
}

void DisplayUIPlayers(WindowManager* window)
{

}

void DestroyPlayers(void)
{
	players_ui_manager->destroy(&players_ui_manager);
	sfRectangleShape_destroy(player_shape[0]);
	sfRectangleShape_destroy(player_shape[1]);
}