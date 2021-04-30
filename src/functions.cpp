#include "functions.hpp"

float clamp(float value, float min, float max)
{
	if(value < min) return min;
	
	if(value > max) return max;
	
	return value;
}

float lerp(float a, float b, float t)
{
	return ( b - a ) * t + a;
}

uint32_t colorLerp(uint32_t color1, uint32_t color2, float t)
{
	t = clamp(t, 0.f, 1.f);
	
	uint8_t r = static_cast<uint8_t>(lerp( color1 >> 24        ,  color2 >> 24        , t));
	uint8_t g = static_cast<uint8_t>(lerp((color1 >> 16) & 0xFF, (color2 >> 16) & 0xFF, t));
	uint8_t b = static_cast<uint8_t>(lerp((color1 >> 8 ) & 0xFF, (color2 >> 8 ) & 0xFF, t));
	uint8_t a = static_cast<uint8_t>(lerp(color1         & 0xFF, color2         & 0xFF, t));
	
	return (r << 24) + (g << 16) + (b << 8) + a;
}

float randf()
{
	return (rand() % 10000) * .0001f;
}