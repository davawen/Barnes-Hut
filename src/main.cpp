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

int main(int argc, char *argv[])
{	
	int BODY_COUNT = 5000;
	float INITIAL_VELOCITY = 100.f;
	
	std::cout << "Enter body count (" << BODY_COUNT << ")\n-> ";
	if(std::cin.peek() != '\n')
		std::cin >> BODY_COUNT;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	
	std::cout << "Initial velocity (" << INITIAL_VELOCITY << ")\n-> ";
	if(std::cin.peek() != '\n')
		std::cin >> INITIAL_VELOCITY;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	
	std::cout << "\nApplication Started!";
		
	#pragma region UI Related Buisness
	
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Barnes Hut Simulation");
	
	const sf::Vector2f originalViewSize = sf::Vector2f(2000.f, 2000.f);
	sf::View view = sf::View(sf::Vector2f(500, 500), originalViewSize);
	window.setView(view);
	
	sf::Font font;
	if(!font.loadFromFile(std::string(argv[0]) + "/../Montserrat-Regular.ttf"))
	{
		return -1; //Failed to load font
	}
	
	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color::Green);
	text.setString("");
	
	#pragma endregion
	
	#pragma region Setup simulation
	
	auto quadtree = Quad(sf::Vector3f(-20000.f, -20000.f, -20000.f), 40000.f);
	
	std::vector<Body *> bodies;
	bodies.reserve( BODY_COUNT );
	
	for(int i = 0; i < BODY_COUNT; i++)
	{
		//Generate sphere coordinate
		float u = randf();
		float v = randf();
		
		float theta = u * 2.f * PI;
		float phi = acosf(2.f * v - 1.f);
		
		float radius = powf(randf(), 1.f / 3.f) * 1000.f; //Cube root for uniform distribution
		
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);
		float sinPhi = sinf(phi);
		float cosPhi = cosf(phi);
		
		auto bodyPosition = sf::Vector3f(radius * sinPhi * cosTheta, radius * sinPhi * sinTheta, radius * cosPhi);
		

		float speed = sqrtf( INITIAL_VELOCITY / radius);
		sf::Vector3f initialVelocity = speed * sf::Vector3f(bodyPosition.y, -bodyPosition.x, 0.f); //Normalize vector
		
		bodies.push_back(
			new Body(
				bodyPosition + sf::Vector3f(500.f, 500.f, 500.f),
				initialVelocity,
				rand() % 100 + 50
			)
		);
	}
	
	#pragma endregion
	
	#pragma region Misc
	
	sf::Clock deltaClock;

	float realDt = 0.f;
	float runningDt = 0.f;
	
	bool isRunning = false;
	bool isDrawingQuadtree = false;
	
	#pragma endregion
	
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
		
		#pragma region Get deltatime
		
		realDt = deltaClock.restart().asSeconds();
		runningDt = isRunning ? realDt : 0.f;
		

		text.setPosition(window.mapPixelToCoords(sf::Vector2i(5, 5)));
		text.setScale(view.getSize().x / originalViewSize.x, view.getSize().y / originalViewSize.y);
		
		text.setString("FPS: " + std::to_string(1.f / realDt));
		
		#pragma endregion
		
		window.draw(text);
		
		window.display();
	}

	return 0;
}