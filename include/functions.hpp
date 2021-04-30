/*--------  Header Guard  --------*/
#ifndef __COLOR_H__
#define __COLOR_H__

// This is essentially a group of crap utility functions

#include <iostream>
#include <math.h>
#include <algorithm>

#include <SFML/Graphics.hpp>

float clamp(float value, float min, float max);

float lerp(float a, float b, float t);

uint32_t colorLerp(uint32_t color1, uint32_t color2, float t);

float randf();

#endif /* __COLOR_H__ */