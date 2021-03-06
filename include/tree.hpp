/*--------  Header Guard  --------*/
#ifndef __TREE_H__
#define __TREE_H__

#include <vector>
#include <iostream>
#include <list>

#include <SFML/Graphics.hpp>

#include "body.hpp"
#include "functions.hpp"

struct Quad
{
	private:
		void clean();
	
	public:
		sf::Vector3f p;
		float size; // Gonna assume its square
		
		enum State { empty, leaf, divided };
		
		State state : 2;
		
		sf::Vector3f centerOfMass;
		float mass;
		
		Quad *nodes[8];

		Quad();
		
		Quad(const sf::Vector3f &p, const float &size);
		
		void cleanRoot();
		
		void insert(Body *body);
		
		void subdivide();
		
		bool inBoundary(const sf::Vector3f &pos);
		
		void draw(sf::RenderWindow &window);
};

#endif /* __TREE_H__ */