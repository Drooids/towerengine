#version 330

uniform vec3 light_ambient_color_uni;

uniform sampler2D depth_tex_uni;
uniform sampler2D diffuse_tex_uni;
uniform sampler2D self_illumination_tex_uni;

in vec2 uv_coord_var;

out vec4 color_out;


void main(void)
{
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	if(depth == 1.0)
		discard;
		
	ivec2 texel_uv = ivec2(uv_coord_var * textureSize(diffuse_tex_uni, 0).xy);

	vec3 diffuse = texelFetch(diffuse_tex_uni, texel_uv, 0).rgb;		
	vec3 self_illumination = texelFetch(self_illumination_tex_uni, texel_uv, 0).rgb;
		
	vec3 color = light_ambient_color_uni * diffuse.rgb + self_illumination;
	
	color_out = vec4(color, 1.0);
}