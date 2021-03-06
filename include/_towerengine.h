#ifndef __TOWERENGINE_H
#define __TOWERENGINE_H

class tMesh;
struct tTriangle;
class tMaterial;
struct tVertex;
class tEntity;
class tWorld;
class tObject;
class tCamera;
class tPointLight;
class tPointLightShadow;
class tDirectionalLight;
class tDirectionalLightShadow;
class tRenderer;
class tDefaultRenderer;
class tDeferredRenderer;
class tForwardRenderer;
class t2DRenderer;

#include "vector2.h"
#include "vector.h"
#include "boundingbox.h"
#include "matrix3.h"
#include "matrix4.h"
#include "transform.h"
#include "utils.h"
#include "buffers.h"
#include "shader.h"
#include "texture.h"
#include "face_shader.h"
#include "geometry_pass_shader.h"
#include "depth_pass_shader.h"
#include "simple_forward_shader.h"
#include "standard_forward_shader.h"
#include "refraction_shader.h"
#include "2d_sprite_shader.h"
#include "gbuffer.h"
#include "comparable.h"

#include "no_op_shader.h"
#include "screen_shader.h"
#include "lighting_shader.h"
#include "ambient_lighting_shader.h"
#include "directional_lighting_shader.h"
#include "point_lighting_shader.h"
#include "ssao_lighting_shader.h"

#include "cube_map_blur_shader.h"
#include "skybox_shader.h"
#include "point_shadow_shader.h"
#include "directional_shadow_shader.h"
#include "post_process_shader.h"
#include "fog_shader.h"
#include "ssao_shader.h"
#include "ssao_blur_shader.h"
#include "color_shader.h"
#include "particle_shader.h"

#include "skybox.h"
#include "material.h"
#include "simple_forward_material.h"
#include "refraction_material.h"
#include "material_ibo.h"
#include "material_manager.h"
#include "mesh.h"
#include "entity.h"
#include "asset.h"
#include "engine.h"
#include "object.h"
#include "meshobject.h"
#include "utilobjects.h"
#include "heightmap.h"
#include "terrain.h"
#include "terrainobject.h"
#include "particle.h"

#include "culling.h"
#include "camera.h"
#include "point_light.h"
#include "directional_light.h"
#include "renderspace.h"
#include "point_light_shadow.h"
#include "directional_light_shadow.h"
#include "refection_probe.h"
#include "world.h"
#include "scene_object.h"
#include "scene.h"
#include "ssao.h"
#include "renderer.h"
#include "forward_renderer.h"
#include "deferred_renderer.h"
#include "default_deferred_renderer.h"
#include "default_forward_renderer.h"
#include "vr_deferred_renderer.h"
#include "vr_forward_renderer.h"
#include "2d_renderer.h"
#include "timgui.h"

#include "vr_context.h"
#include "vr_context_test.h"

#endif
