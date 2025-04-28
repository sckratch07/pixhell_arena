/*
	Author: GRALLAN Yann

	Description: An advanced game engine for CSFML

	Date: 2025/01/22

	MIT License

	Copyright (c) 2025 GRALLAN Yann


	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Particles.h"
#include "MemoryManagement.h"

typedef struct Particle Particle;
struct Particle
{
	sfVector2f m_position;
	sfVector2f m_direction;
	float m_rotation;
	float m_speed;
	float m_despawn_timer;
	float m_despawn_time;
};


struct Particles_Data
{
	stdList* m_particle_list;

	sfTexture* m_texture;

	sfCircleShape* m_vanilla_rendeder;
	sfRectangleShape* m_texture_renderer;

	ParticleParam m_parameters;

	float m_life_timer;
	float m_spawn_timer;

};

static sfBool HasFadingType(Particles* particles, FadingTypes type)
{
	return (particles->_Data->m_parameters.fading_flags & type) == type;
}

static Particle* CreateParticle(Particles* particles)
{
	Particle* particle = calloc_d(Particle, 1);
	assert(particle);

	particle->m_position = particles->_Data->m_parameters.position;
	particle->m_speed = particles->_Data->m_parameters.speed + (float)rand_float(0, particles->_Data->m_parameters.random_speed_boost);
	particle->m_despawn_time = particles->_Data->m_parameters.despawn_time;
	particle->m_rotation = (float)rand_float(0, particles->_Data->m_parameters.random_spawn_rotation);
	particle->m_despawn_timer = 0.f;
	particle->m_direction = GetVectorFromAngle(sfVector2f_Create(0, 0), 1, particles->_Data->m_parameters.angle_direction + (float)rand_float(-particles->_Data->m_parameters.angle_spawn_spread, particles->_Data->m_parameters.angle_spawn_spread));


	return particle;
}


static sfBool UpdateParticle(Particles* particles, Particle* particle, float delta_time)
{
	particle->m_despawn_timer += delta_time;
	if (particle->m_despawn_timer > particle->m_despawn_time)
	{
		return sfTrue;
	}

	particle->m_position = AddVector2f(particle->m_position, MultiplyVector2f(particle->m_direction, particle->m_speed * delta_time));
	particle->m_rotation += particles->_Data->m_parameters.rotation * delta_time;
	return sfFalse;
}

static sfBool ParticlesHasFinish(Particles* particles)
{
	sfBool listIsEmpty = particles->_Data->m_particle_list->size(particles->_Data->m_particle_list) == 0;
	if (particles->_Data->m_parameters.type == ALWAYS)
		return sfFalse;
	if (particles->_Data->m_life_timer > particles->_Data->m_parameters.life_time && particles->_Data->m_parameters.type == LIFE_TIME && listIsEmpty)
		return sfTrue;
	if (particles->_Data->m_parameters.type == ONE_TIME && listIsEmpty)
		return sfTrue;
	if (particles->_Data->m_parameters.type == NONE && listIsEmpty)
		return sfTrue;
	return sfFalse;
}

static void ParticlesDestroy(Particles** particles)
{
	Particles* holder = *particles;
	for (int i = 0; i < holder->_Data->m_particle_list->size(holder->_Data->m_particle_list); i++)
	{
		Particle** it_ = ((Particle**)holder->_Data->m_particle_list->getData(holder->_Data->m_particle_list, i));
		Particle* it = *it_; DetrackerCalloc(it);
	};
	holder->_Data->m_particle_list->destroy(&holder->_Data->m_particle_list);
	free_d(holder->_Data);
	free_d(holder);
	holder = NULL;
}


static void UpdateParticles(Particles* particles, float deltaTime)
{
	if (deltaTime == 0.f)
		deltaTime = DeltaTime;

	Particles_Data* data = particles->_Data;

	data->m_life_timer += deltaTime;
	data->m_spawn_timer += deltaTime;
	if ((data->m_spawn_timer > data->m_parameters.spawn_time && (data->m_parameters.type == ALWAYS ||
		data->m_parameters.type == LIFE_TIME && data->m_life_timer < data->m_parameters.life_time)) ||
		data->m_parameters.type == ONE_TIME)
	{
		if (data->m_parameters.type == ONE_TIME)
			data->m_parameters.type = NONE;

		for (int i = 0; i < data->m_parameters.spawn_count; i++)
		{
			Particle* tmp = CreateParticle(particles);
			data->m_particle_list->push_back(data->m_particle_list, &tmp);
		}
		data->m_spawn_timer = 0;
	}

	FOR_EACH_LIST_POINTER(data->m_particle_list, Particle*, i, it,
		if (UpdateParticle(particles, it, deltaTime))
		{
			free_d(it);
			data->m_particle_list->erase(data->m_particle_list, i);
			i--;
		}
			);
}


static Particles* CreateParticles(ParticleParam parameters, int point_count)
{
	Particles* particles = calloc_d(Particles, 1);
	assert(particles);
	Particles_Data* data = calloc_d(Particles_Data, 1);
	assert(data);

	data->m_particle_list = STD_LIST_CREATE_POINTER(Particle*, 0);
	data->m_texture = NULL;
	data->m_texture_renderer = NULL;
	data->m_vanilla_rendeder = NULL;
	parameters.angle_spawn_spread = (float)abs(parameters.angle_spawn_spread) / 2;
	data->m_parameters = parameters;


	particles->_Data = data;

	particles->Update = &UpdateParticles;
	particles->Destroy = &ParticlesDestroy;
	particles->HasFinish = &ParticlesHasFinish;

	return particles;
}



ParticleParam CreateDefaultParam(ParticlesTypes type, sfVector2f position, float direction, float speed)
{
	return (ParticleParam) {
		.type = type,
			.position = position,
			.origin = sfVector2f_Create(0, 0),
			.radius = 25.f,
			.angle_direction = direction,
			.angle_spawn_spread = 0.f,
			.speed = speed,
			.rotation = 0.f,
			.random_spawn_rotation = 0.f,
			.color = CreateColor(255, 255, 255, 255),
			.despawn_time = 5.f,
			.spawn_time = 0.2f,
			.spawn_count = 1,
			.life_time = 10.f
	};
}

Particles* CreateVanillaParticles(ParticleParam parameters, int point_count)
{
	Particles* particles = CreateParticles(parameters, point_count);


	sfCircleShape* renderer = sfCircleShape_create();
	sfCircleShape_setPosition(renderer, parameters.position);
	sfCircleShape_setOrigin(renderer, parameters.origin);
	sfCircleShape_setRadius(renderer, parameters.radius);
	sfCircleShape_setFillColor(renderer, parameters.color);
	sfCircleShape_setPointCount(renderer, point_count);

	particles->_Data->m_vanilla_rendeder = renderer;
	return particles;
}



Particles* CreateTextureParticles(ParticleParam parameters, sfTexture* texture, sfIntRect texture_rect)
{
	Particles* particles = CreateParticles(parameters, 0);

	if (texture_rect.width == 0 && texture_rect.height == 0)
	{
		texture_rect.width = sfTexture_getSize(texture).x;
		texture_rect.height = sfTexture_getSize(texture).y;
	}
	particles->_Data->m_texture = texture;
	sfRectangleShape* renderer = sfRectangleShape_create();
	sfRectangleShape_setSize(renderer, sfVector2f_Create((float)texture_rect.width, (float)texture_rect.height));
	sfRectangleShape_setOrigin(renderer, parameters.origin);
	sfRectangleShape_setFillColor(renderer, parameters.color);
	sfRectangleShape_setTexture(renderer, texture, sfFalse);
	sfRectangleShape_setTextureRect(renderer, texture_rect);

	particles->_Data->m_texture_renderer = renderer;

	return particles;
}


void sfRenderWindow_drawParticles(sfRenderWindow* render_window, Particles* particles, sfRenderStates* state)
{
	Particles_Data* data = particles->_Data;
	if (data->m_vanilla_rendeder)
	{
		FOR_EACH_LIST_POINTER(data->m_particle_list, Particle*, i, it,
			{
				sfCircleShape_setPosition(data->m_vanilla_rendeder, it->m_position);
				sfCircleShape_setRotation(data->m_vanilla_rendeder, it->m_rotation);
				sfBool fadingCanStart = particles->_Data->m_parameters.fading_start_time < it->m_despawn_timer;
				float scaleFactor = fadingCanStart ? LERP(1.f, 0.f, (it->m_despawn_timer - particles->_Data->m_parameters.fading_start_time) / it->m_despawn_time - particles->_Data->m_parameters.fading_start_time) : 1.f;
				sfCircleShape_setScale(data->m_vanilla_rendeder, HasFadingType(particles, FADING_BY_SIZE) && fadingCanStart ? sfVector2f_Create(scaleFactor, scaleFactor) : sfVector2f_Create(1, 1));
				unsigned char r = LERP(particles->_Data->m_parameters.fading_color.r, particles->_Data->m_parameters.color.r, scaleFactor);
				unsigned char g = LERP(particles->_Data->m_parameters.fading_color.g, particles->_Data->m_parameters.color.g, scaleFactor);
				unsigned char b = LERP(particles->_Data->m_parameters.fading_color.b, particles->_Data->m_parameters.color.b, scaleFactor);
				unsigned char a = LERP(particles->_Data->m_parameters.fading_color.a, particles->_Data->m_parameters.color.a, scaleFactor);
				sfCircleShape_setFillColor(data->m_vanilla_rendeder, HasFadingType(particles, FADING_BY_COLOR) && fadingCanStart ? CreateColor(r, g, b, a) : particles->_Data->m_parameters.color);

				sfRenderWindow_drawCircleShape(render_window, data->m_vanilla_rendeder, state);
			}
		);
	}
	else if (data->m_texture_renderer && data->m_texture)
	{
		FOR_EACH_LIST_POINTER(data->m_particle_list, Particle*, i, it,
			{
				sfRectangleShape_setPosition(data->m_texture_renderer, it->m_position);
				sfRectangleShape_setRotation(data->m_texture_renderer, it->m_rotation);
				sfBool fadingCanStart = particles->_Data->m_parameters.fading_start_time < it->m_despawn_timer;
				float scaleFactor = fadingCanStart ? LERP(1.f, 0.f, (it->m_despawn_timer - particles->_Data->m_parameters.fading_start_time) / it->m_despawn_time - particles->_Data->m_parameters.fading_start_time) : 1.f;
				sfRectangleShape_setScale(data->m_texture_renderer, HasFadingType(particles, FADING_BY_SIZE) && fadingCanStart ? sfVector2f_Create(scaleFactor, scaleFactor) : sfVector2f_Create(1, 1));
				unsigned char r = LERP(particles->_Data->m_parameters.fading_color.r, particles->_Data->m_parameters.color.r, scaleFactor);
				unsigned char g = LERP(particles->_Data->m_parameters.fading_color.g, particles->_Data->m_parameters.color.g, scaleFactor);
				unsigned char b = LERP(particles->_Data->m_parameters.fading_color.b, particles->_Data->m_parameters.color.b, scaleFactor);
				unsigned char a = LERP(particles->_Data->m_parameters.fading_color.a, particles->_Data->m_parameters.color.a, scaleFactor);
				sfRectangleShape_setFillColor(data->m_texture_renderer, HasFadingType(particles, FADING_BY_COLOR) && fadingCanStart ? CreateColor(r, g, b, a) : particles->_Data->m_parameters.color);

				sfRenderWindow_drawRectangleShape(render_window, data->m_texture_renderer, state);
			}
		);
	}

}

void sfRenderTexture_drawParticles(sfRenderTexture* render_texture, Particles* particles, sfRenderStates* state)
{
	Particles_Data* data = particles->_Data;
	if (data->m_vanilla_rendeder)
	{
		FOR_EACH_LIST_POINTER(data->m_particle_list, Particle*, i, it,
			{
				sfCircleShape_setPosition(data->m_vanilla_rendeder, it->m_position);
				sfCircleShape_setRotation(data->m_vanilla_rendeder, it->m_rotation);
				sfBool fadingCanStart = particles->_Data->m_parameters.fading_start_time < it->m_despawn_timer;
				float scaleFactor = fadingCanStart ? LERP(1.f, 0.f, (it->m_despawn_timer - particles->_Data->m_parameters.fading_start_time) / (it->m_despawn_time - particles->_Data->m_parameters.fading_start_time)) : 1.f;
				sfCircleShape_setScale(data->m_vanilla_rendeder, HasFadingType(particles, FADING_BY_SIZE) && fadingCanStart ? sfVector2f_Create(scaleFactor, scaleFactor) : sfVector2f_Create(1, 1));
				unsigned char r = LERP(particles->_Data->m_parameters.fading_color.r, particles->_Data->m_parameters.color.r, scaleFactor);
				unsigned char g = LERP(particles->_Data->m_parameters.fading_color.g, particles->_Data->m_parameters.color.g, scaleFactor);
				unsigned char b = LERP(particles->_Data->m_parameters.fading_color.b, particles->_Data->m_parameters.color.b, scaleFactor);
				unsigned char a = LERP(particles->_Data->m_parameters.fading_color.a, particles->_Data->m_parameters.color.a, scaleFactor);
				sfCircleShape_setFillColor(data->m_vanilla_rendeder, HasFadingType(particles, FADING_BY_COLOR) && fadingCanStart ? CreateColor(r, g, b, a) : particles->_Data->m_parameters.color);

				sfRenderTexture_drawCircleShape(render_texture, data->m_vanilla_rendeder, state);
			}
		);
	}
	else if (data->m_texture_renderer && data->m_texture)
	{
		FOR_EACH_LIST_POINTER(data->m_particle_list, Particle*, i, it,
			{
				sfRectangleShape_setPosition(data->m_texture_renderer, it->m_position);
				sfRectangleShape_setRotation(data->m_texture_renderer, it->m_rotation);
				sfBool fadingCanStart = particles->_Data->m_parameters.fading_start_time < it->m_despawn_timer;
				float scaleFactor = fadingCanStart ? LERP(1.f, 0.f, (it->m_despawn_timer - particles->_Data->m_parameters.fading_start_time) / (it->m_despawn_time - particles->_Data->m_parameters.fading_start_time)) : 1.f;
				sfRectangleShape_setScale(data->m_texture_renderer, HasFadingType(particles, FADING_BY_SIZE) && fadingCanStart ? sfVector2f_Create(scaleFactor, scaleFactor) : sfVector2f_Create(1, 1));
				unsigned char r = LERP(particles->_Data->m_parameters.fading_color.r, particles->_Data->m_parameters.color.r, scaleFactor);
				unsigned char g = LERP(particles->_Data->m_parameters.fading_color.g, particles->_Data->m_parameters.color.g, scaleFactor);
				unsigned char b = LERP(particles->_Data->m_parameters.fading_color.b, particles->_Data->m_parameters.color.b, scaleFactor);
				unsigned char a = LERP(particles->_Data->m_parameters.fading_color.a, particles->_Data->m_parameters.color.a, scaleFactor);
				sfRectangleShape_setFillColor(data->m_texture_renderer, HasFadingType(particles, FADING_BY_COLOR) && fadingCanStart ? CreateColor(r, g, b, a) : particles->_Data->m_parameters.color);

				sfRenderTexture_drawRectangleShape(render_texture, data->m_texture_renderer, state);
			}
		);
	}
}
