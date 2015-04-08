
#include "towerengine.h"

tRenderer::tRenderer(int width, int height, tWorld *world)
{
	geometry_pass_shader = new tGeometryPassShader();
	geometry_pass_shader->Init();
	//SetCurrentFaceShader(geometry_pass_shader);

	directional_lighting_shader = new tDirectionalLightingShader();
	directional_lighting_shader->Init();

	point_lighting_shader = new tPointLightingShader();
	point_lighting_shader->Init();

	ambient_lighting_shader = new tAmbientLightingShader();
	ambient_lighting_shader->Init();

	skybox_shader = new tSkyBoxShader();
	skybox_shader->Init();

	point_shadow_shader = new tPointShadowShader();
	point_shadow_shader->Init();

	point_shadow_blur_shader = new tPointShadowBlurShader();
	point_shadow_blur_shader->Init();

	directional_shadow_shader = new tDirectionalShadowShader();
	directional_shadow_shader->Init();

	directional_shadow_blur_shader = new tDirectionalShadowBlurShader();
	directional_shadow_blur_shader->Init();

	color_shader = new tColorShader();
	color_shader->Init();

	post_process_shader = new tPostProcessShader();
	post_process_shader->Init();

	this->screen_width = width;
	this->screen_height = height;

	this->world = world;

	ssao = 0;
	ssao_lighting_shader = 0;
	ssao_shader = 0;

	fxaa_enabled = false;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &color_tex);

	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screen_width, screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	gbuffer = new tGBuffer(screen_width, screen_height, fbo, 1);

	screen_quad_vao = new tVAO();
	screen_quad_vbo = new tVBO<float>(2, screen_quad_vao, 4);

	float *screen_quad_data = screen_quad_vbo->GetData();
	screen_quad_data[0] = 0.0; screen_quad_data[1] = 1.0;
	screen_quad_data[2] = 0.0; screen_quad_data[3] = 0.0;
	screen_quad_data[4] = 1.0; screen_quad_data[5] = 0.0;
	screen_quad_data[6] = 1.0; screen_quad_data[7] = 1.0;

	screen_quad_vbo->AssignData();

	point_light_shadow_limit = -1;

}

tRenderer::~tRenderer(void)
{
	delete ssao;
	delete gbuffer;

	delete geometry_pass_shader;
	delete ambient_lighting_shader;
	delete directional_lighting_shader;
	delete point_lighting_shader;
	delete ssao_lighting_shader;
	delete ssao_shader;

	delete skybox_shader;
	delete point_shadow_shader;
	delete point_shadow_blur_shader;
	delete directional_shadow_shader;
	delete directional_shadow_blur_shader;
	delete color_shader;
	delete post_process_shader;
}

void tRenderer::InitSSAO(int kernel_size, float radius, int noise_tex_size)
{
	if(!ssao_lighting_shader)
	{
		ssao_lighting_shader = new tSSAOLightingShader();
		ssao_lighting_shader->Init();
	}

	if(!ssao_shader)
	{
		ssao_shader = new tSSAOShader();
		ssao_shader->Init();
	}

	delete ssao;
	ssao = new tSSAO(this, kernel_size, radius, noise_tex_size);
}

void tRenderer::Render(GLuint dst_fbo)
{
	tCamera *camera = world->GetCamera();

	camera->SetAspect((float)screen_width / (float)screen_height);

	world->FillRenderSpaces(point_light_shadow_limit);

	world->RenderShadowMaps(this);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GeometryPass();

	if(ssao)
		ssao->Render(); // binds its own fbo

	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // rebind fbo
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	LightPass();

	ForwardPass();

	glBindFramebuffer(GL_FRAMEBUFFER, dst_fbo);


	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.1, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0); //pos x, y, z, to x, y, z

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	post_process_shader->Bind();
	post_process_shader->SetFXAA(fxaa_enabled);
	post_process_shader->SetTextures(color_tex, depth_tex, screen_width, screen_height);

	glBegin(GL_QUADS);
	glVertex2f(0.0, 1.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 0.0);
	glVertex2f(1.0, 1.0);
	glEnd();

	tShader::Unbind();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void tRenderer::GeometryPass(void)
{
	tCamera *camera = world->GetCamera();

	gbuffer->Bind();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	camera->SetModelviewProjectionMatrix();

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	SetCurrentFaceShader(geometry_pass_shader);
	BindCurrentFaceShader();

	world->GetCameraRenderSpace()->GeometryPass(this);
}

void tRenderer::LightPass(void)
{
	int i;

	tCamera *camera = world->GetCamera();
	tSkyBox *sky_box = world->GetSkyBox();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	glDisable(GL_BLEND);

	camera->SetModelviewProjectionMatrix();

	if(sky_box)
		sky_box->Paint(this, camera->GetPosition());


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.1, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0); //pos x, y, z, to x, y, z

	glDisable(GL_DEPTH_TEST);


	if(ssao)
	{
		ssao_lighting_shader->Bind();
		ssao_lighting_shader->SetGBuffer(gbuffer);
		ssao_lighting_shader->SetSSAOTexture(ssao->GetSSAOTexture());
		ssao_lighting_shader->SetAmbientLight(world->GetAmbientColor());
	}
	else
	{
		ambient_lighting_shader->Bind();
		ambient_lighting_shader->SetGBuffer(gbuffer);
		ambient_lighting_shader->SetAmbientLight(world->GetAmbientColor());
	}

	RenderScreenQuad();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);


	directional_lighting_shader->Bind();
	directional_lighting_shader->SetGBuffer(gbuffer);
	directional_lighting_shader->SetCameraPosition(camera->GetPosition());

	for(i=0; i<world->GetDirectionalLightsCount(); i++)
	{
		world->GetDirectionalLight(i)->InitRenderLighting(directional_lighting_shader);
		RenderScreenQuad();
	}


	point_lighting_shader->Bind();
	point_lighting_shader->SetGBuffer(gbuffer);
	point_lighting_shader->SetCameraPosition(camera->GetPosition());

	for(i=0; i<world->GetPointLightsCount(); i++)
	{
		world->GetPointLight(i)->InitRenderLighting(point_lighting_shader);
		RenderScreenQuad();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void tRenderer::ForwardPass(void)
{
	world->GetCamera()->SetModelviewProjectionMatrix();

	world->GetCameraRenderSpace()->ForwardPass(this);
}


void tRenderer::ChangeSize(int width, int height)
{
	if(screen_width == width && screen_height == height)
		return;

	screen_width = width;
	screen_height = height;

	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	gbuffer->ChangeSize(width, height);

	if(ssao)
		ssao->ChangeScreenSize(width, height);

	glBindTexture(GL_TEXTURE_2D, 0);
}


void tRenderer::RenderScreenQuad(void)
{
	screen_quad_vao->Bind();
	screen_quad_vbo->SetAttribute(tShader::vertex_attribute, GL_FLOAT);
	screen_quad_vao->Draw(GL_QUADS, 0, 4);
}


















