#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <ctime>

constexpr float PI = 3.14159265359;


#include <SFML/Graphics.hpp>

#include "body.hpp"
#include "tree.hpp"
#include "functions.hpp"

int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Barnes Hut Simulation");
	
	sf::View view = sf::View(sf::Vector2f(500, 500), sf::Vector2f(2000, 2000));
	
	window.setView(view);
	
	auto quadtree = Quad(sf::Vector3f(-2000.f, -2000.f, -2000.f), 5000.f);
	
	std::vector<Body *> bodies;
	bodies.reserve( 3000 );
	
	for(int i = 0; i < 3000; i++)
	{
		//Generate sphere coordinate
		float u = randf();
		float v = randf();
		
		float theta = u * 2.f * PI;
		float phi = acosf(2.f * v - 1.f);
		
		float radius = powf(randf(), 1.f / 3.f) * 600.f; //Cube root for uniform distribution
		
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);
		float sinPhi = sinf(phi);
		float cosPhi = cosf(phi);
		
		auto bodyPosition = sf::Vector3f(radius * sinPhi * cosTheta, radius * sinPhi * sinTheta, radius * cosPhi);
		

		float speed = sqrtf(100.f / radius);
		sf::Vector3f initialVelocity = speed * sf::Vector3f(bodyPosition.y, -bodyPosition.x, 0.f); //Normalize vector
		
		bodies.push_back(
			new Body(
				bodyPosition + sf::Vector3f(500.f, 500.f, 500.f),
				initialVelocity,
				rand() % 10 + 10
			)
		);
	}
	
	sf::Clock deltaClock;
	
	bool isRunning = false;
	bool isDrawingQuadtree = false;
	
	float realDt = 0.f;
	float runningDt = 0.f;
	
	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::Resized:
					view.setSize(sf::Vector2f(event.size.width * 2.f, event.size.height * 2.f));
					window.setView(view);
					break;
				case sf::Event::MouseWheelMoved:
				{
					sf::Vector2u windowSize = window.getSize();
					
					//_x + (_w - (_w*zoom))*ox, _y + (_h - (_h*zoom))*oy
					sf::Vector2i __mousePosRelative = sf::Mouse::getPosition( window );
					sf::Vector2f __origin( static_cast<float>( __mousePosRelative.x ) / windowSize.x - .5, static_cast<float>( __mousePosRelative.y ) / windowSize.y - .5 );

					float __zoomAmount = 1 - static_cast<float>( event.mouseWheel.delta ) * .05f;

					sf::Vector2f __viewSize = view.getSize();

					view.move( ( __viewSize.x - __viewSize.x * __zoomAmount ) * __origin.x, ( __viewSize.y - __viewSize.y * __zoomAmount ) * __origin.y );

					view.zoom( __zoomAmount );

					window.setView( view );
				}
					break;
				case sf::Event::KeyPressed:
					switch(event.key.code)
					{
						case sf::Keyboard::S:
							isRunning = !isRunning;
							break;
						case sf::Keyboard::Q:
							isDrawingQuadtree = !isDrawingQuadtree;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
		
		window.clear();
		
		if(isDrawingQuadtree) quadtree.draw( window );
		
		for(auto &body : bodies)
		{
			body->gravity(&quadtree, runningDt);
		}
		
		quadtree.cleanRoot();
		
		for(auto &body : bodies)
		{
			body->update(runningDt);
			body->draw( window );

			quadtree.insert( body );
		}
		
		window.display();
		
		realDt = deltaClock.restart().asSeconds();
		runningDt = isRunning ? realDt : 0.f;
		
		// std::cout << "FPS: " << 1.f / realDt << std::endl;
	}

	return 0;
}