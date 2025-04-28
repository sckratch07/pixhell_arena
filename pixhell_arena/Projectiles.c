#include "Projectiles.h"
#include "stdlib.h"

stdList* all_projectiles;

void CreateProjectile(sfVector2f position, sfVector2f direction, ProjectileType type, float speed, float damage)
{
	ProjectileInfo* projectile = (ProjectileInfo*)malloc(sizeof(ProjectileInfo));
	projectile->position = position;
	projectile->direction = direction;
	projectile->type = type;
	projectile->speed = speed;
	projectile->damage = damage;
	all_projectiles->push_back(all_projectiles, projectile);
}

void InitProjectiles(void)
{
	all_projectiles = stdList_Create(sizeof(ProjectileInfo), 0);
}

void UpdateProjectiles(void)
{
	ProjectileInfo* projectile;
	for (int i = 0; i < all_projectiles->size(all_projectiles); i++)
	{
		projectile = all_projectiles->getData(all_projectiles, i);
		projectile->position = AddVector2f(projectile->position, MultiplyVector2f(projectile->direction, projectile->speed * DeltaTime));

		if (projectile->position.x < 0 || projectile->position.x > 1920 || projectile->position.y < 0 || projectile->position.y > 1080)
		{
			all_projectiles->erase(all_projectiles, i);
			i--;
		}
	}
}

void DisplayProjectiles(WindowManager* window)
{
	ProjectileInfo* projectile;
	sfCircleShape* shape = sfCircleShape_create();
	sfCircleShape_setRadius(shape, 5.f);
	sfCircleShape_setFillColor(shape, (sfColor) { 255, 255, 0, 255 });
	sfCircleShape_setOrigin(shape, sfVector2f_Create(5.f, 5.f));

	for (int i = 0; i < all_projectiles->size(all_projectiles); i++)
	{
		projectile = all_projectiles->getData(all_projectiles, i);
		sfCircleShape_setPosition(shape, projectile->position);
		window->DrawCircleShape(window, shape, NULL);
	}

	sfCircleShape_destroy(shape);
}

void DestroyProjectiles(void)
{
	all_projectiles->destroy(&all_projectiles);
}