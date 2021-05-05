#include "body.hpp"

#include "tree.hpp"

float Body::G = 200.f;
float Body::precision = 3.f;

Body::Body(const sf::Vector3f &position, const sf::Vector3f &initialVelocity, float mass)
	: Body(                    position,                     initialVelocity,       mass, 0xFFFFFFFF) {}

Body::Body(const sf::Vector3f &position, const sf::Vector3f &initialVelocity, float mass, uint32_t color)
{
	this->position = position;
	this->velocity = initialVelocity;
	
	this->mass = mass;
	
	this->shape = sf::Vertex(sf::Vector2f(position.x, position.y), sf::Color(color));
	
	// float colorAmount = randf();
}

void Body::gravity(Quad *quadtree, const float &dt)
{
	switch(quadtree->state)
	{
		/// Do nothing if its empty
		case Quad::State::empty:
			return;
			break;
		/// If its a leaf, calculate force as normal
		case Quad::State::leaf:
		{
			auto diff = quadtree->centerOfMass - position;
			
			double invDst = 1.f / sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			
			if(invDst > 1) return;
			
			diff *= static_cast<float>( invDst ); // Normalize difference
			
			// Multiply all scalars together to minimize component wise multiplication
			
			// float forceValue = G * quadtree->mass / sqDst;
			// sf::Vector2f force = forceValue * diff;
			// velocity += force * dt;
			
			velocity += static_cast<float>( G * quadtree->mass * dt * invDst * invDst ) * diff;
		}
			break;
		case Quad::State::divided:
		{
			auto diff = quadtree->centerOfMass - position;
			
			double invDst = 1.f / sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			
			float theta = quadtree->size * invDst;
			
			if(theta < precision)
			{
				// Calculate far field force
				diff *= static_cast<float>(invDst); // Normalize difference
				
				velocity += static_cast<float>( G * quadtree->mass * dt * invDst * invDst ) * diff;
			}
			else
			{
				for(auto &node : quadtree->nodes)
				{
					gravity(node, dt);
				}
			}
		}
			break;
	}
}

void Body::update(float dt)
{
	position += velocity * dt;
	
	shape.position = sf::Vector2f(position.x, position.y);
}

void Body::draw(sf::RenderWindow &window)
{
	window.draw(&shape, 1, sf::PrimitiveType::Points);
}