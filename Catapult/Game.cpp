// Author: Dr Noel O'Hara
// SFML Starter for an awesome game you can make!
#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 

#include <SFML/Graphics.hpp>
// I use a couple of h files from thor library.
//https://github.com/Bromeon/Thor
#include "VectorAlgebra2D.h"


#include <iostream>
#include <stdlib.h> 
#include <time.h> 
#include <list>
enum PlayerState { ready, drawingLine, Jump };
class Game
{
public:
	//create Window
	sf::RenderWindow window;
	sf::View view;
	float randomNum = 0;

	sf::Vector2f lineStart{ 150,450};
	sf::RectangleShape catapult;
	sf::CircleShape playerShape;
	sf::Vector2f moveForce;
	PlayerState playerState = PlayerState::ready;
	float gravityScale = 20;
	sf::Vector2f velocity = { 0,0 };
	float playerMaxSpeed = 600 - lineStart.y;
	sf::Vector2f gravity{ 0.0f, 9.8f * gravityScale };
	sf::Vector2f normalised;
	int score{ 0 }; // keeps track of the players score
	sf::Font font; // main screen font
	sf::Text scoreText; // shows the players score on screen
	sf::RectangleShape target;
	sf::Vector2f targetPos;
	bool showTarget{ true }; // bool to control if the target is on screen or not
	sf::Vector2f offset;
	

	Game()
	{
		window.create(sf::VideoMode(800, 600), "Untitled Physics Games");
	}
	
	void init()
	{

		view = window.getDefaultView();
		playerShape.setRadius(20);
		playerShape.setPosition(160, 480);
		playerShape.setOrigin(sf::Vector2f(20, 20));
		catapult.setSize(sf::Vector2f{ 40,600 - lineStart.y });
		catapult.setPosition(lineStart.x - (catapult.getSize().x / 2.f), lineStart.y);
		catapult.setFillColor(sf::Color::Green);
		sf::Vector2f normalised = thor::unitVector(playerShape.getPosition());
		float l = thor::length(gravity);
		float r = thor::toDegree(10.0);
		//setup target
		target.setSize(sf::Vector2f{ 50,50 });
		target.setFillColor(sf::Color::Magenta);
		offset = sf::Vector2f{ static_cast<float>(rand() % 200 - 100), static_cast<float>(rand() % 200 - 100) };
		targetPos = (sf::Vector2f{ 600 + offset.x,200 + offset.y});
		target.setPosition(targetPos);
		
		//setup text
		font.loadFromFile("ariblk.ttf");
		scoreText.setFont(font);
		scoreText.setString("Score: 0");
		thor::setLength(normalised, 100.0f);
		thor::rotate(normalised, 10.0f);

	}
	void run()
	{
	
		sf::Time timePerFrame = sf::seconds(1.0f / 60.0f);


		sf::Time timeSinceLastUpdate = sf::Time::Zero;

		
		sf::Clock clock;

		clock.restart();

		while (window.isOpen())
		{
			
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}

			
			timeSinceLastUpdate += clock.restart();

			

			if (timeSinceLastUpdate > timePerFrame)
			{

				if (playerState == ready)
				{
					if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
					{
						playerState = drawingLine;
						showTarget = true;
						target.setPosition(targetPos);
					}
				}
				if (playerState == drawingLine)
				{
					if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
					{
						sf::Vector2f localPosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
						sf::Vector2f newVelocity;
						newVelocity = playerShape.getPosition() - lineStart;
						newVelocity.x *= 3.5;
						newVelocity.y *= 3.5;

						if (thor::length(moveForce) < playerMaxSpeed)
						{
							velocity = -newVelocity;
						}
						else {
							thor::setLength(moveForce, playerMaxSpeed);
							velocity = -newVelocity;
						}

						playerState = Jump;
						gravity.y = 9.8 * gravityScale;
					}
				}
				if (playerState == Jump)
				{
					playerState = ready;
				}


				
				velocity += (gravity * timeSinceLastUpdate.asSeconds());

				playerShape.move(velocity.x * timeSinceLastUpdate.asSeconds(), velocity.y * timeSinceLastUpdate.asSeconds());
				//collision detection for target
				if (showTarget)
				{
					if (target.getGlobalBounds().intersects(playerShape.getGlobalBounds()))
					{
						showTarget = false;
						score++;
						scoreText.setString("Score: " + std::to_string(score));
						target.setPosition(-1000, -1000);
						offset = sf::Vector2f{ static_cast<float>(rand() % 200 - 100), static_cast<float>(rand() % 200 - 100) };
						targetPos = (sf::Vector2f{ 600 + offset.x,200 + offset.y });
					}
				}

				if (playerShape.getPosition().y > 600 - playerShape.getRadius())
				{
					velocity.y *= -1;
					playerShape.setPosition(playerShape.getPosition().x, 600 - playerShape.getRadius());
				}
				else if (playerShape.getPosition().y < 0)
				{
					velocity.y *= -1;
					playerShape.setPosition(playerShape.getPosition().x, 0);
				}

				if (playerShape.getPosition().x < 0)
				{
					velocity.x *= -1;
					playerShape.setPosition(0, playerShape.getPosition().y);
				}
				else if (playerShape.getPosition().x > 800 - playerShape.getRadius())
				{
					velocity.x *= -1;
					playerShape.setPosition(800 - playerShape.getRadius(), playerShape.getPosition().y);
				}
				
				window.clear();
				if (playerState == drawingLine)
				{
					sf::Vector2f localPosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
					if (thor::squaredLength(localPosition - lineStart) > (playerMaxSpeed * playerMaxSpeed))
					{
						sf::Vector2f newLocal = thor::unitVector(sf::Vector2f{ localPosition - lineStart }) * playerMaxSpeed;
						localPosition = lineStart + newLocal;
					}
					sf::Vertex line[] =
					{
						sf::Vertex(localPosition),
						sf::Vertex(lineStart)
					};
					playerShape.setPosition(localPosition);
					window.draw(line, 2, sf::Lines);
				}
				window.draw(playerShape);
				window.draw(catapult);
				if (showTarget)
					window.draw(target);
				window.draw(scoreText);
				
				window.display();

				
				timeSinceLastUpdate = sf::Time::Zero;
			}
		}
	}
};


int main()
{
	srand(static_cast<unsigned>(time(nullptr)));

	Game game;
	

	game.init();

	game.run();



	return 0;
}