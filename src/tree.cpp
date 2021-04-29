#include "include/tree.hpp"

Quad::Quad() : Quad( sf::Vector3f(), sf::Vector3f() ) {}

Quad::Quad( const sf::Vector3f &p, const sf::Vector3f &size)
{
	this->p = p;
	this->size = size;

	this->centerOfMass = p + size / 2.f;
	this->mass = 0;
	
	this->state = State::empty;
	
	// std::cout << this->divided;
	
	nodes[0] = NULL;
	nodes[1] = NULL;
	nodes[2] = NULL;
	nodes[3] = NULL;
	nodes[4] = NULL;
	nodes[5] = NULL;
	nodes[6] = NULL;
	nodes[7] = NULL;
}

void Quad::subdivide()
{
	sf::Vector3f halfSize = size / 2.f;
	
	for(int i = 0; i <= 1; i++)
	{
		for(int j = 0; j <= 1; j++)
		{
			for(int k = 0; k <= 1; k++)
			{
				nodes[i*4 + j*2 + k] = new Quad(
					sf::Vector3f(
						p.x + size.x * i * .5f,
						p.y + size.y * j * .5f,
						p.z + size.z * k * .5f
					),
					halfSize
				);
			}
		}
	}
	
	state = State::divided;
}

void Quad::clean()
{
	if(state != State::divided) return;
	
	for(auto &node : nodes)
	{
		node->clean();
		
		delete node;
	}
}

void Quad::cleanRoot()
{
	// Offset quadtree to stay at center of mass
	// p -= (( p + size / 2.f ) - centerOfMass);
	
	centerOfMass = p + size / 2.f;
	mass = 0;
	
	if(state != State::divided) return;
	
	for(auto &node : nodes)
	{
		node->clean();
		
		delete node;
		
		node = NULL;
	}

	state = State::empty;
}

void Quad::insert(Body *body)
{
	if(body == NULL)
		return;
		
	if(!inBoundary( body->position ))
		return;

	// std::cout << "Working with body: " << body << "\n";
	
	centerOfMass = (centerOfMass * mass + body->position * body->mass) / (mass + body->mass);
	mass += body->mass;
	
	switch(state)
	{
		case State::empty:
			state = State::leaf;
			break;
		case State::leaf:
			subdivide();
		// Notice there is no break
		case State::divided:
			for(auto &node : nodes)
			{
				node->insert(body);
			}
			break;
	}
}

bool Quad::inBoundary(const sf::Vector3f &pos)
{
	return pos.x >= p.x          &&
		   pos.x <= p.x + size.x &&
		   pos.y >= p.y          &&
		   pos.y <= p.y + size.y &&
		   pos.z >= p.z          &&
		   pos.z <= p.z + size.z;
}

void Quad::draw(sf::RenderWindow &window)
{
	auto shape = sf::VertexArray(sf::PrimitiveType::LineStrip, 5);
	
	shape[0].position = { p.x, p.y };
	shape[1].position = sf::Vector2f(p.x + size.x, p.y);
	
	sf::Vector3f p2 = p + size;
	
	shape[2].position = { p2.x, p2.y };
	shape[3].position = sf::Vector2f(p.x, p.y + size.y);
	shape[4].position = { p.x, p.y };
	
	if(state == State::divided)
	{
		for(auto &node : nodes)
		{
			node->draw(window);
		}
	}
	
	window.draw(shape);
}