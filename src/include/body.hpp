/*--------  Header Guard  --------*/
#ifndef __BODY_H__
#define __BODY_H__

#include <vector>
#include <math.h>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "functions.hpp"

class Quad;

class Body
{
	private:
		sf::VertexArray shape;
		
		static float G;
		static float precision;
		
		float size;
		
	public:
		sf::Vector3f position;
		sf::Vector3f velocity;
		float radius;
		float mass;
		
		Body(const sf::Vector3f &position, float mass);
		
		void gravity(Quad *quadtree, const float &dt);
		void gravity(std::vector<Body *> &bodies, const float &dt);
		void update(float dt);
		void draw(sf::RenderWindow &window);
};

#endif /* __BODY_H */