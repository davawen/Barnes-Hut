#include "include/body.hpp"

#include "include/tree.hpp"

float Body::G = 1.f;
float Body::precision = 3.f;

Body::Body(const sf::Vector3f &position, float mass)
{
	this->position = position;
	this->velocity = sf::Vector3f();
	
	this->mass = mass;
	this->size = sqrtf(mass);
	
	// this->shape = sf::CircleShape(1.f);
	// this->shape.setOrigin(1.f, 1.f);
	this->shape = sf::VertexArray(sf::PrimitiveType::Points, 1);

	float colorAmount = randf();
	for(int i = 0; i < 1; i++)
	{
		this->shape[i].color = sf::Color(colorLerp(0x0000FFFF, 0x7FFFFFFF, colorAmount));
	}
}

void Body::gravity(Quad *quadtree, const float &dt)
{
	// If this is a leaf, calculate gravity as normal
	// std::cout << "a\n";
	
	if(!quadtree->divided)
	{
		if(quadtree->data == NULL) return;
		
		auto difference = quadtree->data->position - this->position;
		
		float sqDst = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
		if(sqDst <= 2.f) return;
		
		float forceValue = G * quadtree->data->mass / sqDst;
		
		auto force = forceValue * difference;

		// if(forceValue > 100.f)
		// {
		// 	printf( "mass: %f, distance: %f, x: %f, y: %f\n", quadtree->mass, sqrtf(sqDst), force.x, force.y );
		// }

		this->velocity += force * dt;
		
		return;
	}
	
	// std::cout << "b\n";
	
	//Calculate distance to quadtree center
	sf::Vector3f diff = quadtree->centerOfMass - position;
	
	// std::cout << "c\n";
	
	float distance = sqrtf( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );
	
	float theta = quadtree->size.x / distance;
	
	if(theta < precision)
	{
		// Calculate far field force
		float forceValue = this->G * quadtree->mass / ( distance * distance );
		
		auto force = forceValue * diff;
		
		// if(forceValue > 100.f)
		// {
		// 	printf( "mass: %f, distance: %f, x: %f, y: %f\n", quadtree->mass, distance, force.x, force.y );
		// }
		
		// printf("mass: %f, distance: %f, x: %f, y: %f\n", quadtree->mass, distance, force.x, force.y );
		
		velocity += force * dt;
	}
	else
	{
		for(auto &node : quadtree->nodes)
		{
			gravity( node, dt );
		}
	}
}

void Body::gravity(std::vector<Body *> &bodies, const float &dt)
{
	for(auto &body : bodies)
	{
		if(body == this) continue;
		
		auto difference = body->position - this->position;
		
		float sqDst = difference.x * difference.x + difference.y * difference.y;
		
		if(sqDst <= 4.f) continue;
		
		auto force = this->G * body->mass * difference / sqDst;
		
		std::cout << dt << std::endl;
		
		this->velocity += force * dt;
	}
}

void Body::update(float dt)
{
	position += velocity * dt;
	
	shape[0].position = { position.x, position.y };
	
	// float s = size / (position.z*.001 + 1.f);
	
	// shape[0].position = { position.x - s / 2.f, position.y - s / 2.f };
	// shape[1].position = { position.x + s / 2.f, position.y - s / 2.f };
	// shape[2].position = { position.x + s / 2.f, position.y + s / 2.f };
	// shape[3].position = { position.x - s / 2.f, position.y + s / 2.f };
}

void Body::draw(sf::RenderWindow &window)
{
	window.draw(shape);
}