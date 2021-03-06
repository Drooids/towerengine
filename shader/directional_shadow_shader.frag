#version 330

uniform bool diffuse_tex_enabled_uni;
uniform sampler2D diffuse_tex_uni;

in float moment1_var;
in vec2 uv_var;

in vec3 debug_var;

out vec4 color_out;

void main(void)
{
	if(diffuse_tex_enabled_uni)
	{
		if(texture(diffuse_tex_uni, uv_var).a < 0.5)
			discard;
	}
		
	color_out = vec4(moment1_var, moment1_var * moment1_var, 0.0, 1.0);
}
