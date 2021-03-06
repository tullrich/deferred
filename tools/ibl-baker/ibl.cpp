#include "ibl.h"
#include "cubemap.h"

static void ir_face_view_mat4x4(mat4x4 out, CubeMapFaces face) {
	mat4x4_zero(out);
	out[3][3] = 1.0f;
	switch(face) {
	case CUBEMAP_FRONT: out[0][0] = out[1][1] = 1.0f; out[2][2] = -1.0f;  return;
	case CUBEMAP_BACK: out[0][0] = -1.0f; out[1][1] = out[2][2] = 1.0f;  return;

	case CUBEMAP_TOP: out[0][0] = 1.0f; out[2][1] = out[1][2] = -1.0f;  return;
	case CUBEMAP_BOTTOM: out[0][0] = out[1][2] = out[2][1] = 1.0f; return; //w

	case CUBEMAP_LEFT: out[1][1] = 1.0f; out[2][0] = out[0][2] = -1.0f; return; // w
	case CUBEMAP_RIGHT: out[1][1] = out[0][2] = out[2][0] = 1.0f; return; // w
	}
}

static void dir_for_fragment(vec3 out, int face, float u, float v, IrradianceCompute *ir) {
	vec4 view = {(float)u, (float)v, .0f, 1.0f};
	vec4 result;
	mat4x4_mul_vec4(result, ir->inv_viewproj[face], view);
	vec3_scale(out, result, 1.0f/result[3]); // divide by w
	vec3_norm(out, out);
}

static void sample_cubemap(vec3 out, const vec3 dir, gli::texture_cube& cubemap, unsigned int width, unsigned int height ) {
	const int axes[6][3] = {
		{ 2, 0, 1 }, // CUBEMAP_FRONT +z
		{ 2, 0, 1 }, // CUBEMAP_BACK -z
		{ 1, 0, 2 }, // CUBEMAP_TOP +y
		{ 1, 0, 2 }, // CUBEMAP_BOTTOM -y
		{ 0, 2, 1 }, // CUBEMAP_LEFT +x
		{ 0, 2, 1 }  // CUBEMAP_RIGHT -x
	};
	const float signs[6][2] = {
		{ -1.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, -1.0f },
		{ 1.0f, 1.0f },
		{ -1.0f, 1.0f }
	};
	int face;
	if (fabsf(dir[0]) > fabsf(dir[1])) {
		if (fabsf(dir[0]) > fabsf(dir[2])) {
			face = (dir[0] > 0.0f) ? CUBEMAP_LEFT : CUBEMAP_RIGHT;
		} else {
			face = (dir[2] > 0.0f) ? CUBEMAP_FRONT : CUBEMAP_BACK;
		}
	} else if (fabsf(dir[1]) > fabsf(dir[2])) {
		face = (dir[1] > 0.0f) ? CUBEMAP_TOP : CUBEMAP_BOTTOM;
	} else {
		face = (dir[2] > 0.0f) ? CUBEMAP_FRONT : CUBEMAP_BACK;
	}
	const int *axis = axes[face];
	const float *sign = signs[face];
	float u = (1.0f + sign[0]*dir[axis[1]]/fabsf(dir[axis[0]]))/2.0f;
	float v = (1.0f + sign[1]*dir[axis[2]]/fabsf(dir[axis[0]]))/2.0f;

  glm::u8vec3 value = cubemap.load<glm::u8vec3>(gli::extent2d((int)(u * width), (int)(v * height)), face, 0);
	out[0] = value.r / 255.0f;
	out[1] = value.g / 255.0f;
	out[2] = value.b / 255.0f;
}

static float area_element(float x, float y) {
	//http://www.rorydriscoll.com/2012/01/15/cubemap-texel-solid-angle/
	return atan2f(x * y, sqrtf(x * x + y * y + 1));
}

static float texcoord_solid_angle(float u, float v, float size) {
	float InvResolution = 1.0f / size;

	// U and V are the -1..1 texture coordinate on the current face.
	// Get projected area for this texel
	float x0 = u - InvResolution;
	float y0 = v - InvResolution;
	float x1 = u + InvResolution;
	float y1 = v + InvResolution;
	float SolidAngle = area_element(x0, y0) - area_element(x0, y1) - area_element(x1, y0) + area_element(x1, y1);

	return SolidAngle;
}

static void radiance_sample(vec3 result, vec3 eye_dir, int face, float su, float sv, float solid_angle, IrradianceCompute *ir) {
	vec3 ray;
	dir_for_fragment(ray, face, su, sv, ir);

	float lambert = fmaxf(vec3_mul_inner(ray, eye_dir), 0.0f);
	float term = lambert*solid_angle;

	vec3 sample;
	sample_cubemap(sample, ray, ir->cubemap, ir->width, ir->height);
	vec3_scale(sample, sample, term);
	vec3_add(result, result, sample);
}

static void compute_irradiance_fragment(vec3 out, int face, float u, float v, IrradianceCompute *ir) {
	vec3 eye_dir;
	dir_for_fragment(eye_dir, face, u , v, ir);

#if 1
	vec3 result = { 0.0f };
	for (unsigned int sv = 0; sv < ir->height; sv++) {
		for (unsigned int su = 0; su < ir->width; su++) {
			float ndc_su = 2.0f*(su+0.5f)/(float)ir->width - 1.0f;
			float ndc_sv = 2.0f*(sv+0.5f)/(float)ir->height - 1.0f;
			float solid_angle = texcoord_solid_angle(ndc_su, ndc_sv, (float)ir->height);
			radiance_sample(result, eye_dir, 0, ndc_su, ndc_sv, solid_angle, ir);
			radiance_sample(result, eye_dir, 1, ndc_su, ndc_sv, solid_angle, ir);
			radiance_sample(result, eye_dir, 2, ndc_su, ndc_sv, solid_angle, ir);
			radiance_sample(result, eye_dir, 3, ndc_su, ndc_sv, solid_angle, ir);
			radiance_sample(result, eye_dir, 4, ndc_su, ndc_sv, solid_angle, ir);
			radiance_sample(result, eye_dir, 5, ndc_su, ndc_sv, solid_angle, ir);
		}
	}
	vec3_scale(out, result, 1.0f/(float)M_PI);
	//vec3_scale(out, result, 1.0f/result[3]);
#else
	vec4 result ={0.0f};
#define RANGE_0_1(val) (.5f*val+0.5f)
	out[0] = RANGE_0_1(eye_dir[0]);
	out[1] = RANGE_0_1(eye_dir[1]);
	out[2] = RANGE_0_1(eye_dir[2]);
#endif
}

// convolve radiance map to an irradiance map and write out
int ibl_compute_irradiance_map(gli::texture_cube& env_map, const char* output_path) {
	int width = 128;//env_map.extent().x;
	int height = 128;//env_map.extent().y;

	// Allocate output buffer
	gli::texture_cube irr_map(gli::FORMAT_RGB8_UNORM_PACK8, gli::extent2d(width, height), 1);

	// Prepare input
	IrradianceCompute ir;
	ir.cubemap = env_map;
	ir.height = height;
	ir.width = width;
	mat4x4 proj;
	mat4x4_perspective(proj, (float)M_PI/2.0f, width/(float)height, 1.0f, 2.0f);
	mat4x4_invert(ir.inv_proj, proj);
	for (int i = 0; i < 6; i++) {
		mat4x4 view, inv_view, viewproj;
		ir_face_view_mat4x4(view, (CubeMapFaces)i);
		mat4x4_invert(inv_view, view);
		mat4x4_mul(viewproj, proj, inv_view);
		mat4x4_invert(ir.inv_viewproj[i], viewproj);
	}

	// Convolve step
	for (int i = 0; i < 6; i++) {
		for (int v = 0; v < height; v++) {
			for (int u = 0; u < width; u++) {
				vec3 irr;
				compute_irradiance_fragment(irr, i, 2.0f*(u+0.5f)/(float)width - 1.0f, 2.0f*(v+0.5f)/(float)height - 1.0f, &ir);
        irr_map.store(gli::extent2d(u, v), i, 0, glm::u8vec3((char)(255 * irr[0]), (char)(255 * irr[1]), (char)(255 * irr[2])));
			}
			printf("Convolved pixel: %f\n", 100.0f * (v+1)/(float)height);
		}
	}

	// Write output
  if (save_cubemap(irr_map, output_path)) {
		printf("Error writing irradiance map for file '%s'\n", output_path);
		return 1;
	}

	printf("Wrote irradiance map '%s'\n", output_path);
	return 0;
}
