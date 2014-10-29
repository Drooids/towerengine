
#ifndef _GBUFFER_H
#define _GBUFFER_H


class tGBuffer
{
	public:
		enum GBUFFER_TEXTURE_TYPE
		{
			POSITION_TEX,
			DIFFUSE_TEX,
			NORMAL_TEX,
			SPECULAR_TEX,
			SPECULAR_EXPONENT_TEX
		};

		static const int tex_count = 5;

	private:
		GLuint fbo;
		GLuint tex[tex_count];

		int first_attachment;

		GLenum *draw_buffers;


	public:
		tGBuffer(int width, int height, GLuint fbo, int first_attachment);
		~tGBuffer(void);

		void ChangeSize(int width, int height);

		void Bind(void);

		GLuint GetTexture(GBUFFER_TEXTURE_TYPE type)	{ return tex[type]; }

		static int GetTexCount(void)					{ return tex_count; }
};


#endif
