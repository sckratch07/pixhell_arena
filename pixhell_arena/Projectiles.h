#pragma once
#include "InGame.h"

typedef enum 
{
	PLAYER = 0,
	ENEMY = 1,
	NORMAL_BULLET = 2,
}ProjectileType;

typedef struct
{
	sfVector2f position;
	sfVector2f direction;
	ProjectileType type;
	float speed;
	float damage;
}ProjectileInfo;

void CreateProjectile(sfVector2f position, sfVector2f direction, ProjectileType type, float speed, float damage);
void InitProjectiles(void);
void UpdateProjectiles(void);
void DisplayProjectiles(WindowManager* window);
void DestroyProjectiles(void);