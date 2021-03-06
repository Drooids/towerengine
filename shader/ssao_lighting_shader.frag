#version 330

uniform sampler2D depth_tex_uni;
uniform sampler2D ssao_tex_uni;

in vec2 uv_coord_var;

out vec4 color_out;

void main(void)
{
	float depth = texture(depth_tex_uni, uv_coord_var).x;

	if(depth == 1.0)
		discard;

	float occlusion = texture(ssao_tex_uni, uv_coord_var).r;
	
	color_out = vec4(vec3(1.0) * occlusion, 1.0);
}