
#ifndef _POINT_LIGHT_H
#define _POINT_LIGHT_H

class tPointLight: public tComparable<float>
{
	private:
		tVector pos;
		tVector color;
		float distance;

		bool shadow_enabled;
		tPointLightShadow *shadow;
		bool shadow_invalid;

		float sorting_value;

	public:
		tPointLight(tVector pos, tVector color, float distance);

		void SetPosition(tVector pos)		{ this->pos = pos; }
		void SetColor(tVector color)	{ this->color = color; }
		void SetDistance(float dist)	{ this->distance = dist; }

		tVector GetPosition(void)			{ return pos; }
		tVector GetColor(void)				{ return color; }
		float GetDistance(void)				{ return distance; }
		bool GetShadowEnabled(void)			{ return shadow_enabled; }
		tPointLightShadow *GetShadow(void)	{ return shadow; }

		void InitShadow(int size, bool blur_enabled = false, float blur_size = 0.003);
		void RenderShadow(tRenderer *renderer);

		void InitRenderLighting(tPointLightingShader *shader);

		void SetSortingValue(float v)	{ sorting_value = v; }
		float GetSortingValue(void)		{ return sorting_value; }

		void InvalidateShadow(void)		{ shadow_invalid = true; }
		bool GetShadowInvalid(void)		{ return shadow_invalid; }
};

#endif
