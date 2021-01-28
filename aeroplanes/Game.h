/// <summary>
/// author Pete Lowe May 2019
/// you need to change the above line or lose marks
/// </summary>
#ifndef GAME_HPP
#define GAME_HPP
/// <summary>
/// include guards used so we don't process this file twice
/// same as #pragma once
/// Don't forget the endif at the bottom
/// </summary>
#include <SFML/Graphics.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

const int HEIGHT = 800;
const int WIDTH = 1000;

class Game
{
public:
	Game();
	~Game();
	/// <summary>
	/// main method for game
	/// </summary>
	void run();

private:

	void processEvents();
	void processKeys(sf::Event t_event);
	void processMousePress(sf::Event t_event);
	void processMouseRelease(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();
	void movePlanes();
	void keepOnScreen(sf::Vector2f &t_location);
	bool checkforCollision(sf::Vector2f t_locationOne, float t_radiusOne,
							sf::Vector2f t_locationTwo, float t_radiusTwo);
	void animateExplosion();
	
	void setupFontAndText();
	void setupSprites();

	sf::RenderWindow m_window; // main SFML window
	sf::Font m_ArialBlackfont; // font used by message
	sf::Text m_welcomeMessage; // text used for message on screen
	sf::Texture m_AreoplaneTexture; // texture used for planes
	sf::Texture m_skyTexture; // texture for sky
	sf::Sprite m_bigPlaneSprite; // sprite used for big plane
	sf::Sprite m_smallPlaneSprite; // sprite used for small plane
	sf::Sprite m_skySprite; // sprite used for sky
	sf::Texture m_explosionTexture; // texture for explosion
	sf::Sprite m_explosionSprite; // sprite used for explosion

	int m_explosionframe = 0;  // current frame for explosion animation
	float m_explosionTimer = 0.0f; // curretn value of timer for frame change fo explosion
	float m_explosionIncrement = 0.4f; // value of timer step per game frame
	bool m_explosionOn{ false }; // is the explosion running
	bool m_displayMessage{ true }; // do we display the help message on screen

	sf::Vector2f m_bigPlaneLocation;  // location of big plane geometric centre
	sf::Vector2f m_smallPlaneLocation; // location of small plane
	sf::Vector2f m_bigPlaneVelocity{ 1.0f,-1.0f }; // initial velocity of big plane
	sf::Vector2f m_smallPlaneVelocity{ -2.0f, 2.0f }; // initial velocity of small plane
	float m_bigPlaneHeading = 45.0f; // in degrees
	float m_smallPlaneHeading = 225.0f; // heading of small plane
	sf::Vector2f m_firstClick{ 0.0f,0.0f }; // location of mouse down event
	sf::Vector2f m_secondClick{ 0.0f,0.0f }; // locatiion of mouse up event
	
	float m_bigPlaneRadius = 52.0f;   // radius of bounding circle for big plane
	float m_smallPaneRadius = 44.0f; // bounding radius of small plane
	
	bool m_exitGame; // control exiting game

};

#endif // !GAME_HPP

