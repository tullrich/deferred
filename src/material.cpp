#include "common.h"
#include "material.h"
#include "assets.h"
#include "imgui/imgui.h"

static const vec4 Normal_Z_Up = { 0.5f, 0.5f, 1.0f, 0.0f };

int material_initialize_default(Material *out) {
  memset(out, 0, sizeof(Material));
  out->albedo_map = utility_load_texture_constant(White);
  out->normal_map = utility_load_texture_constant(Normal_Z_Up);
  out->height_map = utility_load_texture_constant(White);
  out->metalness_map = utility_load_texture_constant(White);
  out->roughness_map = utility_load_texture_constant(White);
  out->ao_map = utility_load_texture_constant(White);
  out->emissive_map = utility_load_texture_constant(White);
  vec3_swizzle(out->albedo_base, 1.0f);
  out->metalness_base = 0.0f;
  out->roughness_base = 1.0f;
  vec3_swizzle(out->emissive_base, 0.0f);
  out->height_map_scale = 0.0f;
  return 0;
}

int material_load(Material *out, const MaterialDesc *desc) {
  memset(out, 0, sizeof(Material));

  if (!desc->albedo_map_path || !(out->albedo_map = utility_load_texture(GL_TEXTURE_2D, desc->albedo_map_path, desc->use_point_sampling)))
    out->albedo_map = 0;
  if (!desc->normal_map_path || !(out->normal_map = utility_load_texture(GL_TEXTURE_2D, desc->normal_map_path, desc->use_point_sampling)))
    out->normal_map = 0;
  if (!desc->height_map_path || !(out->height_map = utility_load_texture(GL_TEXTURE_2D, desc->height_map_path, desc->use_point_sampling)))
    out->height_map = 0;
  if (!desc->metalness_map_path || !(out->metalness_map = utility_load_texture(GL_TEXTURE_2D, desc->metalness_map_path, desc->use_point_sampling)))
    out->metalness_map = 0;
  if (!desc->roughness_map_path || !(out->roughness_map = utility_load_texture(GL_TEXTURE_2D, desc->roughness_map_path, desc->use_point_sampling)))
    out->roughness_map = 0;
  if (!desc->ao_map_path || !(out->ao_map = utility_load_texture(GL_TEXTURE_2D, desc->ao_map_path, desc->use_point_sampling)))
    out->ao_map = 0;
  if (!desc->emissive_map_path || !(out->emissive_map = utility_load_texture(GL_TEXTURE_2D, desc->emissive_map_path, desc->use_point_sampling)))
    out->emissive_map = 0;
  vec3_dup(out->albedo_base, desc->albedo_base);
  out->metalness_base = desc->metalness_base;
  out->roughness_base = desc->roughness_base;
  vec3_dup(out->emissive_base, desc->emissive_base);
  out->height_map_scale = desc->height_map_scale;
  out->desc = desc;
  return 0;
}

static void texture_map_toggle_gui(const char* label, GLuint *map, const GLuint def) {
  bool use_map = (*map != 0);
  if (ImGui::Checkbox(label, &use_map)) {
    *map = (use_map) ? def : 0;
  }
  ImGui::SameLine(125.0f);
  ImGui::Image((ImTextureID)(intptr_t)(def), ImVec2(75.0f, 75.0f));
}

void material_gui(Material* material) {
  const MaterialDesc* desc = material->desc;
  if (ImGui::BeginCombo("Presets", desc->name, 0)) {
    for (int i = 0; i < gMaterialsCount; i++) {
      if (ImGui::Selectable(gMaterials[i].name, (&gMaterials[i] == desc))) {
        *material = gMaterials[i].material;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::BeginGroup();
    ImGui::PushID("Albedo");
    ImGui::Separator();
    ImGui::Text("Albedo");
    ImGui::Indent();
    ImGui::ColorEdit3("Base", material->albedo_base);
    if (desc->material.albedo_map) {
      texture_map_toggle_gui("Texture", &material->albedo_map, desc->material.albedo_map);
    }
    ImGui::Unindent();
    ImGui::PopID();
  ImGui::EndGroup();
  if (desc->material.normal_map) {
    ImGui::BeginGroup();
      ImGui::PushID("Normal");
      ImGui::Separator();
      ImGui::Text("Normal");
      ImGui::Indent();
      texture_map_toggle_gui("Texture", &material->normal_map, desc->material.normal_map);
      ImGui::Unindent();
      ImGui::PopID();
    ImGui::EndGroup();
  }
  if (desc->material.height_map) {
    ImGui::BeginGroup();
      ImGui::PushID("Height");
      ImGui::Separator();
      ImGui::Text("Height");
      ImGui::Indent();
      texture_map_toggle_gui("Texture", &material->height_map, desc->material.height_map);
      if (desc->material.height_map) {
        ImGui::SliderFloat("Height Scale", &material->height_map_scale,  0.0f, 0.1f);
      }
      ImGui::Unindent();
      ImGui::PopID();
    ImGui::EndGroup();
  }
  ImGui::BeginGroup();
    ImGui::PushID("Roughness");
    ImGui::Separator();
    ImGui::Text("Roughness");
    ImGui::Indent();
    ImGui::SliderFloat("Base", &material->roughness_base,  0.0f, 1.0f);
    if (desc->material.roughness_map) {
      texture_map_toggle_gui("Texture", &material->roughness_map, desc->material.roughness_map);
    }
    ImGui::Unindent();
    ImGui::PopID();
  ImGui::EndGroup();
  ImGui::BeginGroup();
    ImGui::PushID("Metalness");
    ImGui::Separator();
    ImGui::Text("Metalness");
    ImGui::Indent();
    ImGui::SliderFloat("Base", &material->metalness_base,  0.0f, 1.0f);
    if (desc->material.metalness_map) {
      texture_map_toggle_gui("Texture", &material->metalness_map, desc->material.metalness_map);
    }
    ImGui::Unindent();
    ImGui::PopID();
  ImGui::EndGroup();
  if (desc->material.ao_map) {
    ImGui::BeginGroup();
      ImGui::PushID("AO");
      ImGui::Separator();
      ImGui::Text("AO");
      ImGui::Indent();
      texture_map_toggle_gui("Texture", &material->ao_map, desc->material.ao_map);
      ImGui::Unindent();
      ImGui::PopID();
    ImGui::EndGroup();
  }
  ImGui::BeginGroup();
    ImGui::PushID("Emissive");
    ImGui::Separator();
    ImGui::Text("Emissive");
    ImGui::Indent();
    ImGui::ColorEdit3("Base", material->emissive_base);
    if (desc->material.emissive_map) {
      texture_map_toggle_gui("Emissive", &material->emissive_map, desc->material.emissive_map);
    }
    ImGui::Unindent();
    ImGui::PopID();
  ImGui::EndGroup();
}
