#pragma once
#include "common.h"

#include "particles.h"
#include "mesh.h"
#include "material.h"
#include "skybox.h"
#include "light.h"

#define SCENE_MODELS_MAX 256
#define SCENE_EMITTERS_MAX 256

typedef struct
{
	// camera position
	vec3 pos;

	// xyz-euler orientation
	vec3 rot;

	// camera boom length
	float boomLen;

	// camera field of view
	float fovy;

	// view matrix
	mat4x4 view;

	// projection matrix
	mat4x4 proj;

	// view-projection matrix
	mat4x4 viewProj;
} Camera;

typedef struct
{
	// Model translation
	vec3 position;

	// Model euler angle rotation
	vec3 rot;

	// Model uniform scale
	float scale;

	// The geometry to render
	Mesh mesh;

	// The material to render with
	Material material;
} Model;

typedef struct
{
	// Main camera values
	Camera camera;

	// Scene skybox
	Skybox skybox;

	// The global ambient light
	vec3 ambient_color;

	// The intensity of the ambient light
	float ambient_intensity;

	// Main light
	Light light;

	// Models to render
	Model* models[SCENE_MODELS_MAX];

	// Particle emitters to render
	ParticleEmitter* emitters[SCENE_EMITTERS_MAX];
} Scene;

void scene_camera_forward(const Scene* s, vec3 out);
void scene_camera_up(const Scene* s, vec3 out);
