#include "include/body.hpp"

#include "include/tree.hpp"

float Body::G = 1.f;
float Body::precision = 3.f;

float Body::minDepth = 0.f;
float Body::maxDepth = 0.f;

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
	// std::cout << "a\n";
	
	
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
			
			float sqDst = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
			
			if(sqDst <= 1) return;
			
			diff /= sqrtf(sqDst); // Normalize difference
			
			// Minimize component wise multiplication
			// float forceValue = G * quadtree->mass / sqDst;
			// auto force = forceValue * diff;
			// velocity += force * dt;
			
			velocity += (G * quadtree->mass * dt) / sqDst * diff; 
		}
			break;
		case Quad::State::divided:
		{
			auto diff = quadtree->centerOfMass - position;
			
			float distance = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			
			float theta = quadtree->size.x / distance;
			
			if(theta < precision)
			{
				// Calculate far field force
				
				diff /= distance; // Normalize difference
				
				// Minimize component wise multiplication
				
				velocity += ( G * quadtree->mass * dt ) / (distance * distance) * diff;
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
	
	if(position.z < minDepth) minDepth = position.z;
	else if(position.z > maxDepth) maxDepth = position.z;
	
	// shape[0].position = { position.x, position.y };
	

	// float s = size / std::max(position.z * .001f, 0.1f);

	shape[0].position = { position.x - size / 2.f, position.y - size / 2.f };
	shape[1].position = { position.x + size / 2.f, position.y - size / 2.f };
	shape[2].position = { position.x + size / 2.f, position.y + size / 2.f };
	shape[3].position = { position.x - size / 2.f, position.y + size / 2.f };
}

void Body::draw(sf::RenderWindow &window)
{
	for(int i = 0; i < 4; i++)
	{
		this->shape[i].color.a = static_cast<uint8_t>( 0xFF * ( position.z - minDepth ) / ( maxDepth - minDepth ) );
	}
	
	window.draw(shape);
}