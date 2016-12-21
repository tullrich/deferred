#include "particles.h"

static void random_direction(vec3 out) {
	float theta = utility_randomRange(0.0f, M_PI);
	float roe = utility_randomRange(0.0f, M_PI);
	out[0] = cosf(roe)*sinf(theta);
	out[1] = sinf(roe)*sinf(theta);
	out[2] = -cosf(theta);
	if (utility_random01()) {
		vec3_negate_in_place(out);
	}
}

void particle_update(Particle* part, float dt) {
	vec3 deltaPos;
	vec3_scale(deltaPos, part->velocity, dt);
	vec3_add(part->pos, part->pos, deltaPos);
	part->ttl -= dt;

	float min_dt = fmin(part->ttl, dt);

	vec4 dt_color;
	vec4_scale(dt_color, part->delta_color, dt);
	vec4_add(part->color, part->color, dt_color);

	vec3 dt_scale;
	vec3_scale(dt_scale, part->delta_scale, dt);
	vec3_add(part->scale, part->scale, dt_scale);
}

int particle_emitter_initialize(ParticleEmitter *emitter, const ParticleEmitterDesc* def) {
	memset(emitter, 0, sizeof(ParticleEmitter));
	emitter->desc = def;
	if ((emitter->particles = calloc(def->max, sizeof(Particle))) == NULL) {
		return 1; // error: unable to allocate memory
	}
	emitter->max = def->max;
	quat_identity(emitter->rot);
	emitter->scale = 1.0f;
	return 0;
}

int particle_emitter_destroy(ParticleEmitter *emitter) {
	if (!emitter->desc || !emitter->particles) {
		return 1; // error: uninitialized emitter
	}
	free(emitter->particles);
	memset(emitter, 0, sizeof(ParticleEmitter));
	return 0;
}

Particle* particle_emitter_emit_one(ParticleEmitter* emitter) {
	const ParticleEmitterDesc* desc = emitter->desc;
	if (emitter->count >= emitter->max) {
		return NULL; // error: not enough space
	}
	Particle* part = &emitter->particles[emitter->count++];
	memset(part, 0, sizeof(Particle));
	part->ttl = desc->life_time + utility_random_float() * desc->life_time_variance;
	part->scale[0] = part->scale[1] = part->scale[2] = desc->start_scale;
	vec3_swizzle(part->delta_scale, (desc->end_scale - desc->start_scale) / part->ttl);
	random_direction(part->velocity);
	vec3_scale(part->velocity, part->velocity, desc->speed + utility_random_float() * desc->speed_variance);
	vec4_dup(part->color, desc->start_color);
	vec4_sub(part->delta_color, desc->end_color, desc->start_color);
	vec4_scale(part->delta_color, part->delta_color, 1.0f/part->ttl);
	return part;
}

void particle_emitter_burst(ParticleEmitter* emitter, int count) {
	for (int i = 0; i < count; i++) {
		particle_emitter_emit_one(emitter);
	}
}

void particle_emitter_destroy_at_index(ParticleEmitter* emitter, int index) {
	if (index >= 0 && index < emitter->count) {
		if (index != emitter->count-1) {
			// swap end into hole except for last element
			emitter->particles[index] = emitter->particles[emitter->count-1];
		}
		emitter->count--;
	}
}

void particle_emitter_update(ParticleEmitter* emitter, float dt) {
	const ParticleEmitterDesc* desc = emitter->desc;

	// spawn particles
	if (!emitter->muted && desc->spawn_rate > 0) {
		float rate = 1.0f / desc->spawn_rate;
		emitter->time_till_spawn += dt;
		while (emitter->time_till_spawn > rate) {
			particle_emitter_emit_one(emitter);
			emitter->time_till_spawn -= rate;
		}
	}

	// advance particle state
	for(int i = 0; i < emitter->count; i++) {
		Particle* part = &emitter->particles[i];
		particle_update(part, dt);
	}

	// destroy particles
	for (int i = 0; i < emitter->count; i++) {
		Particle* part = &emitter->particles[i];
		if (part->ttl <= 0.0f) {
			particle_emitter_destroy_at_index(emitter, i);
		}
	}

	// depth sort
	//TODO: add vec3 eye param
	//if (emitter->depth_sort && eye) {
	//}
}
