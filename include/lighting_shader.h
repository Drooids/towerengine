
#ifndef _LIGHTING_SHADER_H
#define _LIGHTING_SHADER_H

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE

#include "material.h"

/**
 * Lighting Shader used if ARB_bindless_texture is supported
 */
class tLightingShader : public tScreenShader, public tReflectingShader
{
	private:
		GLint depth_tex_uniform;
		GLint base_color_tex_uniform;
		GLint normal_tex_uniform;
		GLint metallic_roughness_tex_uniform;
		GLint emission_tex_uniform;

		GLint ssao_enabled_uniform;
		GLint ssao_tex_uniform;

		GLint cam_pos_uniform;

		GLint light_ambient_color_uniform;

		GLint reflection_tex1_uniform;
		GLint reflection_tex2_uniform;
		GLint reflection_tex_blend_uniform;

		int reflection_tex1_unit;
		int reflection_tex2_unit;

	public:

		static const int max_point_lights_count = 32;

		tLightingShader(tGBuffer *gbuffer);

		void SetAmbientLight(tVector color);
		void SetCameraPosition(tVector pos);
		void SetSSAO(bool enabled, GLuint64 tex_handle);

		void SetReflectionTextures(GLuint tex1, GLuint tex2, float blend);
};

class tLightingShaderPointLightsBuffer : public tUniformBuffer
{
	public:
		tLightingShaderPointLightsBuffer(void);
		~tLightingShaderPointLightsBuffer(void);

		void UpdateBuffer(std::vector<tPointLight *> lights);

		void Bind(void);
};

#endif


#endif
