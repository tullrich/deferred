cmake_minimum_required( VERSION 2.6 )

# define the path of our additional CMake modules
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
set( OpenGL_GL_PREFERENCE "LEGACY" )

project( Deferred )

find_package( SDL2 REQUIRED )
find_package( OpenGL REQUIRED )
find_package( GLEW REQUIRED )
find_package( GLI REQUIRED )
find_package( FreeImage REQUIRED )

add_subdirectory( thirdparty/stb )
add_subdirectory( tools/ibl-baker )

if ( NOT MSVC )
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/")
endif()

add_executable( Deferred
  src/main.cpp
  src/common.h
  src/common.cpp
  src/utility.h
  src/utility.cpp
  src/gbuffer.h
  src/gbuffer.cpp
  src/deferred.h
  src/deferred.cpp
  src/forward.h
  src/forward.cpp
  src/particles.h
  src/particles.cpp
  src/scene.h
  src/scene.cpp
  src/mesh.h
  src/mesh.cpp
  src/skybox.h
  src/skybox.cpp
  src/material.h
  src/material.cpp
  src/light.h
  src/light.cpp
  src/assets.h
  src/assets.cpp
  src/shadowmap.h
  src/shadowmap.cpp
  src/debug_lines.h
  src/debug_lines.cpp
  src/renderer.h
  src/renderer.cpp
  src/gui.h
  src/gui.cpp
  src/physics_particles.h
  src/physics_particles.cpp
  src/physics_rigidbodies.h
  src/physics_rigidbodies.cpp

  src/imgui/imconfig.h
  src/imgui/imgui.h
  src/imgui/imgui_internal.h
  src/imgui/imgui_demo.cpp
  src/imgui/imgui_draw.cpp
  src/imgui/imgui_widgets.cpp
  src/imgui/imgui.cpp
  src/imgui/imgui_custom_theme.h
  src/imgui/imgui_custom_theme.cpp
  src/imgui/imstb_textedit.h
  src/imgui/imstb_rectpack.h
  src/imgui/imstb_truetype.h
  src/imgui/imgui_impl_opengl3.h
  src/imgui/imgui_impl_opengl3.cpp
  src/imgui/imgui_impl_sdl.h
  src/imgui/imgui_impl_sdl.cpp

  src/imgui/ImGuizmo.h
  src/imgui/ImGuizmo.cpp

  src/tinyobj_loader_c/tinyobj_loader_c.c
)

if ( NOT MSVC )
  set(CMAKE_BUILD_TYPE Debug)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
  endif()

  set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-missing-field-initializers")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  target_link_libraries( Deferred "-lm" )
else()
  target_compile_definitions( Deferred PUBLIC -D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions( Deferred PUBLIC -DTW_STATIC -DTW_NO_LIB_PRAGMA)

include_directories(
  ${GLM_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${STB_INCLUDE_DIR}
  ${GLI_INCLUDE_DIR}
  ${FREEIMAGE_INCLUDE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
)

target_link_libraries(Deferred
  ${SDL2_LIBRARY}
  ${SDL2MAIN_LIBRARY}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${STB_LIBRARIES}
  ${GLI_LIBRARY_DIR}
  ${FREEIMAGE_LIBRARIES}
)
