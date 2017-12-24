#version 130

in vec3 Position;
in vec3 Normal;
#ifdef MESH_VERTEX_UV1
in vec2 Texcoord;
#endif
#ifdef USE_NORMAL_MAP
in vec3 Tangent;
in vec3 Bitangent;
#endif

uniform mat4 ModelView;
uniform mat4 invTModelView;
uniform vec3 AlbedoBase;
uniform vec3 SpecularBase;

#ifdef USE_ALBEDO_MAP
uniform sampler2D AlbedoMap;
#endif
#ifdef USE_NORMAL_MAP
uniform sampler2D NormalMap;
#endif
#ifdef USE_SPECULAR_MAP
uniform sampler2D SpecularMap;
#endif
#ifdef USE_AO_MAP
uniform sampler2D AOMap;
#endif

out vec3 PositionOut;
out vec4 AlbedoOut;
out vec3 NormalOut;
out vec4 SpecularOut;

struct SurfaceOut
{
	vec3 Albedo;
	vec3 Normal;
	vec4 Specular;
	float Occlusion;
};

void SurfaceShaderTextured(out SurfaceOut surface)
{
	vec3 normal;
#ifdef USE_NORMAL_MAP
	mat3 tbn;
	tbn[0] = Tangent;
	tbn[1] = Bitangent;
	tbn[2] = Normal;
	vec3 normalSample = texture(NormalMap, Texcoord).xyz * 2.0f - 1.0f;
	normal = tbn * normalSample;
#else
	normal = Normal;
#endif // USE_NORMAL_MAP
	surface.Normal = normalize(mat3(invTModelView) * normal);

	surface.Albedo = AlbedoBase;
#ifdef USE_ALBEDO_MAP
	surface.Albedo *= texture(AlbedoMap, Texcoord).xyz;
#endif // USE_ALBEDO_MAP

	surface.Specular = vec4(SpecularBase, 0.0f);
#ifdef USE_SPECULAR_MAP
	surface.Specular *= texture(SpecularMap, Texcoord);
#endif // USE_SPECULAR_MAP

#ifdef USE_AO_MAP
	surface.Occlusion = texture(AOMap, Texcoord).x;
#else
	surface.Occlusion = 0.0f;
#endif//  USE_AO_MAP
}

void main()
{
	SurfaceOut surface;
	SurfaceShaderTextured(surface);

	PositionOut = Position;
	AlbedoOut = vec4(surface.Albedo, surface.Occlusion);
	NormalOut = surface.Normal;
	SpecularOut = surface.Specular;
}