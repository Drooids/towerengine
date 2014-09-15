
#include "towerengine.h"

void CPostProcessShader::Init(void)
{
	InitShader(post_process_shader_vert, post_process_shader_frag, "Post Process Shader");
	LinkProgram();

	color_tex_uniform = GetUniformLocation("color_tex_uni");
	depth_tex_uniform = GetUniformLocation("depth_tex_uni");
	tex_pixel_uniform = GetUniformLocation("tex_pixel_uni");
}

void CPostProcessShader::SetTextures(GLuint color, GLuint depth, int width, int height)
{
	glUniform1iARB(color_tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color);

	glUniform1iARB(depth_tex_uniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depth);

	glUniform2fARB(tex_pixel_uniform, 1.0 / (float)width, 1.0 / (float)height);
}
