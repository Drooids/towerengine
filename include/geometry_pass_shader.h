
#ifndef _DEFAULT_FACE_SHADER_H
#define _DEFAULT_FACE_SHADER_H


class tGeometryPassShader : public tFaceShader
{
	private:
		GLint cam_pos_uniform;

		GLint transformation_uniform;

		GLint diffuse_color_uniform;
		GLint specular_color_uniform;
		GLint specular_size_uniform; // TODO: in exponent umbenennen
		GLint self_illumination_color_uniform;

		GLint bump_depth_uniform;

		GLint diffuse_tex_enabled_uniform;
		GLint normal_tex_enabled_uniform;
		GLint specular_tex_enabled_uniform;
		GLint bump_tex_enabled_uniform;
		GLint self_illumination_tex_enabled_uniform;

		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint specular_tex_uniform;
		GLint bump_tex_uniform;
		GLint self_illumination_tex_uniform;

		GLint cube_map_reflection_enabled_uniform;
		GLint cube_map_reflection_color_uniform;
		GLint cube_map_reflection_tex_uniform;

		//GLint clip_uniform;
		//GLint clip_dist_uniform;

	public:
		void Init(void);

		void SetCameraPosition(tVector pos);
		//void SetClip(tVector c, float d);
		void SetDiffuseColor(tVector color);
		void SetSpecularColor(tVector color);
		void SetSpecular(float size);
		void SetDiffuseTexture(bool enabled, GLuint tex = 0);
		void SetSpecularTexture(bool enabled, GLuint tex = 0);
		void SetNormalTexture(bool enabled, GLuint tex = 0);
		void SetBumpTexture(bool enabled, GLuint tex = 0);
		void SetBumpDepth(float depth);
		void SetTransformation(const float m[16]);
		void SetSelfIlluminationColor(tVector color);
		void SetSelfIlluminationTexture(bool enabled, GLuint tex =0);
		void SetCubeMapReflectionEnabled(bool enabled);
		void SetCubeMapReflectionColor(tVector color);
		void SetCubeMapReflectionTexture(GLuint tex);
};


#endif
