#pragma once

#include "common.h"
#include "gbuffer.h"
#include "scene.h"

typedef struct
{
	GLuint program;

	// shader vars
	GLint pos_loc;
	GLint texcoord_loc;

	GLint env_map_loc;

	GLint inv_vp_loc;
} SkyboxShader;

typedef struct
{
	GLuint program;

	// shader vars
	GLint pos_loc;
	GLint normal_loc;
	GLint tangent_loc;
	GLint texcoord_loc;

	GLint albedo_map_loc;
	GLint normal_map_loc;
	GLint metalness_map_loc;
	GLint roughness_map_loc;
	GLint ao_map_loc;

	GLint modelview_loc;
	GLint invTModelview_loc;
	GLint view_loc;
	GLint albedo_base_loc;
	GLint roughness_base_loc;
	GLint metalness_base_loc;
} SurfaceShader;

typedef struct
{
	GLuint program;

	// shader vars
	GLint pos_loc;
	GLint texcoord_loc;

	union
	{
		struct
		{
			GLint gbuffer_position_loc;
			GLint gbuffer_normal_loc;
			GLint gbuffer_albedo_loc;
			GLint gbuffer_roughness_loc;
			GLint gbuffer_metalness_loc;
			GLint gbuffer_depth_loc;
		};
		GLint gbuffer_locs[GBUFFER_ATTACHMENTS_COUNT];
	};
	GLint env_map_loc;

	// shader vars
	GLint ambient_term_loc;
	GLint light_pos_loc;
	GLint light_color_loc;
	GLint light_intensity_loc;
	GLint eye_pos_loc;
	GLint inv_view_loc;

} LightingShader;

typedef struct
{
	GLuint program;

	// shader vars
	GLint pos_loc;
	GLint texcoord_loc;
	GLint gbuffer_render_loc;
	GLint gbuffer_depth_loc;

} DebugShader;

typedef struct
{
	RenderMode render_mode;
	SkyboxShader skybox_shader;
	SurfaceShader surf_shader[2];
	LightingShader lighting_shader;
	DebugShader debug_shader[2];
	GBuffer g_buffer;
} Deferred;

int deferred_initialize(Deferred* d);
void deferred_render(Deferred* d, Scene *s);
