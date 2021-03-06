
#include "towerengine.h"

tSSAO::tSSAO(tDeferredRenderer *renderer, int kernel_size, float radius, int noise_tex_size)
{
	this->renderer = renderer;

	blur_shader = new tSSAOBlurShader();

	render_width = renderer->GetScreenWidth() / 2;
	render_height = renderer->GetScreenHeight() / 2;

	this->kernel_size = kernel_size;
	kernel = new float[kernel_size*3];

	this->radius = radius;

	float scale;
	tVector v;

	for(int i=0; i<kernel_size; i++)
	{
		while(1)
		{
			v.x = RandomFloat(-1.0, 1.0);
			v.y = RandomFloat(-1.0, 1.0);
			v.z = RandomFloat(0.0, 1.0);
			v.Normalize();

			scale = (float)i / (float)kernel_size;
			scale = Mix(0.1, 1.0, scale * scale);
			v *= scale;

			if(v.z > 0.05)
				break;
		}

		memcpy(kernel + i*3, v.v, sizeof(float) * 3);
	}

	this->noise_tex_size = noise_tex_size;
	GLubyte *noise_tex_data = new GLubyte[noise_tex_size * noise_tex_size * 3];
	int x, y;

	for(y=0; y<noise_tex_size; y++)
	{
		for(x=0; x<noise_tex_size; x++)
		{
			noise_tex_data[y*noise_tex_size*3 + x*3 + 0] = (GLubyte)(RandomFloat() * 255.0);
			noise_tex_data[y*noise_tex_size*3 + x*3 + 1] = (GLubyte)(RandomFloat() * 255.0);
			noise_tex_data[y*noise_tex_size*3 + x*3 + 2] = 0.0;
		}
	}

	glGenTextures(1, &noise_tex);
	glBindTexture(GL_TEXTURE_2D, noise_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noise_tex_size, noise_tex_size, 0, GL_RGB, GL_UNSIGNED_BYTE, noise_tex_data);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete [] noise_tex_data;


	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, render_width, render_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	glGenTextures(1, &blur_tex);
	glBindTexture(GL_TEXTURE_2D, blur_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, render_width, render_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, blur_tex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
	tex_handle = 0;
	if(tEngine::GetARBBindlessTextureSupported())
		tex_handle = glGetTextureHandleARB(tex);
	tex_handle_resident = false;
#endif
}

tSSAO::~tSSAO(void)
{
#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
	MakeTextureHandleResident(false);
#endif
	glDeleteTextures(1, &tex);
	glDeleteTextures(1, &blur_tex);
	glDeleteFramebuffers(1, &fbo);
	delete blur_shader;

	delete [] kernel;
}


void tSSAO::ChangeScreenSize(int screen_width, int screen_height)
{
	render_width = screen_width / 2;
	render_height = screen_height / 2;

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, render_width, render_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	glGenTextures(1, &blur_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, render_width, render_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
}


void tSSAO::Render(float viewport_scale_x, float viewport_offset_x)
{
	tSSAOShader *ssao_shader = renderer->GetSSAOShader();
	tVector2 noise_tex_scale;

	tGBuffer *gbuffer = renderer->GetGBuffer();

	noise_tex_scale.x = (float)render_width / (float)noise_tex_size;
	noise_tex_scale.y = (float)render_height / (float)noise_tex_size;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	ssao_shader->Bind();
	ssao_shader->SetKernel(kernel_size, kernel);
	ssao_shader->SetNoiseTex(noise_tex, noise_tex_scale);
	ssao_shader->SetTextures(gbuffer->GetTexture(tGBuffer::DEPTH_TEX), gbuffer->GetTexture(tGBuffer::FACE_NORMAL_TEX));
	ssao_shader->SetMatrices(renderer->GetCurrentRenderingCamera()->GetProjectionMatrix().GetData(), renderer->GetCurrentRenderingCamera()->GetModelViewMatrix().GetData());
	ssao_shader->SetRadius(radius);
	ssao_shader->SetCamera(renderer->GetCurrentRenderingCamera()->GetPosition(), renderer->GetCurrentRenderingCamera()->GetDirection());

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport((int)((float)render_width * viewport_offset_x), 0, (int)((float)render_width * viewport_scale_x), render_height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	renderer->RenderScreenQuad();

	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	blur_shader->Bind();
	blur_shader->SetTexture(tex);
	blur_shader->SetBlurDirection(false);
	renderer->RenderScreenQuad();

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	blur_shader->SetTexture(blur_tex);
	blur_shader->SetBlurDirection(true);
	renderer->RenderScreenQuad();

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
	MakeTextureHandleResident(true);
#endif
}


#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
void tSSAO::MakeTextureHandleResident(bool resident)
{
	if(tex_handle == 0 || tex_handle_resident == resident)
		return;

	if(resident)
		glMakeTextureHandleResidentARB(tex_handle);
	else
		glMakeTextureHandleNonResidentARB(tex_handle);

	tex_handle_resident = resident;
}
#endif


























