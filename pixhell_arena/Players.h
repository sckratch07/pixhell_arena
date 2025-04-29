#pragma once
#include "InGame.h"
#include "Animation.h"

#define PLAYER_NUMBER 2

// #######################################
// #
// #  JOYSTICK L: MOUVEMENT
// #  JOYSTICK R: SCOPE
// #
// #  RT: SHOOT
// #  LT: ATTACK
// #
// #  B: DODGE
// #  
// #######################################

typedef struct PlayerInfo
{
	sfVector2f position;
	sfVector2f direction;
	Animation_Key* anim_key;
	float shoot_timer;
	float attack_timer;
	float dodge_timer;
	float shoot_delay;
	float attack_delay;
	float dodge_delay;
	sfBool is_dodging;
}PlayerInfo;

PlayerInfo players[PLAYER_NUMBER];

void InitPlayers(void);
void UpdatePlayers(void);
void DisplayPlayers(WindowManager* window);
void DisplayUIPlayers(WindowManager* window);
void DestroyPlayers(void);
