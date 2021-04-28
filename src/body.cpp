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
	this->shape = sf::VertexArray(sf::PrimitiveType::Quads, 4);

	float colorAmount = randf();
	for(int i = 0; i < 4; i++)
	{
		this->shape[i].color = sf::Color(colorLerp(0x0000FFFF, 0x7FFFFFFF, colorAmount + (randf() * .02f) - .01f));
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
		if(sqDst <= size*size)
		{
			velocity -= velocity * .01f * dt; // "Friction"
			return;
		}
		
		float forceValue = G * quadtree->data->mass / sqDst;
		
		auto force = forceValue * difference;

		this->velocity += force * dt;
		
		return;
	}
	
	//Calculate distance to quadtree center
	sf::Vector3f diff = quadtree->centerOfMass - position;
	
	float distance = sqrtf( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );
	
	float theta = quadtree->size.x / distance;
	
	if(theta < precision)
	{
		// Calculate far field force
		float forceValue = this->G * quadtree->mass / ( distance * distance );
		
		auto force = forceValue * diff;
		
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

void Body::update(float dt)
{
	position += velocity * dt;
	
	// shape[0].position = { position.x, position.y };
	
	// float s = size / (position.z*.001 + 1.f);
	
	shape[0].position = { position.x - size / 2.f, position.y - size / 2.f };
	shape[1].position = { position.x + size / 2.f, position.y - size / 2.f };
	shape[2].position = { position.x + size / 2.f, position.y + size / 2.f };
	shape[3].position = { position.x - size / 2.f, position.y + size / 2.f };
}

void Body::draw(sf::RenderWindow &window)
{
	window.draw(shape);
}