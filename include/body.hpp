/*--------  Header Guard  --------*/
#ifndef __BODY_H__
#define __BODY_H__

#include <vector>
#include <math.h>
#include <iostream>
#include <Windows.h>

#include <SFML/Graphics.hpp>

#include "functions.hpp"

class Quad;

class Body
{
	private:
		sf::Vertex shape;

		sf::Vector3f velocity;
		
	public:
		static float G;
		static float precision;

		sf::Vector3f position;
		float radius;
		float mass;
		
		Body(const sf::Vector3f &position, const sf::Vector3f &initialVelocity, float mass);
		Body(const sf::Vector3f &position, const sf::Vector3f &initialVelocity, float mass, uint32_t color);
		
		/// Calculate the gravity from all of the bodies in the quadtree
		void gravity(Quad *quadtree, const float &dt);
		
		void update(float dt);
		void draw(sf::RenderWindow &window);
};

#endif /* __BODY_H */