#include "body.hpp"

#include "tree.hpp"

float Body::G = 100.f;
float Body::precision = 3.f;

Body::Body(const sf::Vector3f &position, const sf::Vector3f &initialVelocity, float mass)
{
	this->position = position;
	this->velocity = initialVelocity;
	
	this->mass = mass;
	this->size = sqrtf(mass);
	
	// this->shape = sf::CircleShape(1.f);
	// this->shape.setOrigin(1.f, 1.f);
	this->shape = sf::VertexArray(sf::PrimitiveType::Quads, 4);

	// float colorAmount = randf();
	for(int i = 0; i < 4; i++)
	{
		this->shape[i].color = sf::Color(0x207FFFFF);
	}
}

void Body::gravity(Quad *quadtree, const float &dt)
{
	// If this is a leaf, calculate gravity as normal
	
	switch(quadtree->state)
	{
		/// Do nothing if its empty
		case Quad::State::empty:
			return;
			break;
		/// If its a leaf, calculate force
		case Quad::State::leaf:
		{
			auto diff = quadtree->centerOfMass - position;
			
			double invDst = 1.f / sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			
			if(invDst > 1) return;
			
			diff *= static_cast<float>( invDst ); // Normalize difference
			
			// Minimize component wise multiplication
			// float forceValue = G * quadtree->mass / sqDst;
			// auto force = forceValue * diff;
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
				
				// Minimize component wise multiplication
				
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
	
	// shape[0].position = { position.x, position.y };
	
	
	// float s = size / std::max( powf(1.001f, position.z), 1.f);
	float s = size;
	
	shape[0].position = { position.x - s / 2.f, position.y - s / 2.f };
	shape[1].position = { position.x + s / 2.f, position.y - s / 2.f };
	shape[2].position = { position.x + s / 2.f, position.y + s / 2.f };
	shape[3].position = { position.x - s / 2.f, position.y + s / 2.f };
}

void Body::draw(sf::RenderWindow &window)
{
	window.draw(shape);
}