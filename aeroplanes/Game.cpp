/// <summary>
/// @author Peter Lowe
/// @date May 2019
///
/// two areoplane using vectors as velocities
/// </summary>

#include "Game.h"
#include <iostream>



float vector2fLenght(sf::Vector2f t_vector);

/// <summary>
/// default constructor
/// setup the window properties
/// load and setup the text 
/// load and setup thne image
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ WIDTH, HEIGHT, 32U }, "SFML Game" },
	m_exitGame{false} //when true game will exit
{
	setupFontAndText(); // load font 
	setupSprites(); // load texture
}

/// <summary>
/// default destructor we didn't dynamically allocate anything
/// so we don't need to free it, but mthod needs to be here
/// </summary>
Game::~Game()
{
}


/// <summary>
/// main game loop
/// update 60 times per second,
/// process update as often as possible and at least 60 times per second
/// draw as often as possible but only updates are on time
/// if updates run slow then don't render frames
/// </summary>
void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
#ifdef _DEBUG
			render();
#endif // _DEBUG

			update(timePerFrame); //60 fps
		}
		render(); // as many as possible
	}
}
/// <summary>
/// handle user and system events/ input
/// get key presses/ mouse moves etc. from OS
/// and user :: Don't do game update here
/// </summary>
void Game::processEvents()
{
	sf::Event newEvent;
	while (m_window.pollEvent(newEvent))
	{
		if ( sf::Event::Closed == newEvent.type) // window message
		{
			m_exitGame = true;
		}
		if (sf::Event::KeyPressed == newEvent.type) //user pressed a key
		{
			processKeys(newEvent);
		}
		if (sf::Event::MouseButtonPressed == newEvent.type)
		{
			processMousePress(newEvent);
		}
		if (sf::Event::MouseButtonReleased == newEvent.type)
		{
			processMouseRelease(newEvent);
		}
	}
}


/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(sf::Event t_event)
{
	m_displayMessage = false;
	if (sf::Keyboard::Escape == t_event.key.code)
	{
		m_exitGame = true;
	}
}

/// <summary>
/// start of user input drag
/// </summary>
/// <param name="t_event"></param>
void Game::processMousePress(sf::Event t_event)
{	
		m_firstClick.x = t_event.mouseButton.x;
		m_firstClick.y = t_event.mouseButton.y;
}


/// <summary>
/// end of user input drag
// get vector from start to finish
// taninverse get slope copnvert from radians to degree
// rotate sprite + 90 degrees
// scale vector for screen speed
/// </summary>
/// <param name="t_event">mouse event</param>
void Game::processMouseRelease(sf::Event t_event)
{
	m_secondClick.x = t_event.mouseButton.x;
	m_secondClick.y = t_event.mouseButton.y;
	sf::Vector2f velocity = m_secondClick - m_firstClick;
	float headingRadians = std::atan2(velocity.y, velocity.x);
	float headingDegree = 180.0f * headingRadians / static_cast<float>(M_PI);
	headingDegree += 90.0f;
	
	std::cout << headingRadians << std::endl;
	if (sf::Mouse::Left == t_event.mouseButton.button )
	{
		m_bigPlaneVelocity = velocity / 100.0f;
		m_bigPlaneHeading = headingDegree;
		m_bigPlaneSprite.setRotation(headingDegree);
	}
	if (sf::Mouse::Right == t_event.mouseButton.button)
	{
		m_smallPlaneVelocity = velocity / 50.0f;
		m_smallPlaneHeading = headingDegree;
		m_smallPlaneSprite.setRotation(headingDegree);
	}
}

/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
	{
		m_window.close();
	}
	movePlanes();
	keepOnScreen(m_bigPlaneLocation);
	keepOnScreen(m_smallPlaneLocation);
	if (!m_explosionOn)
	{
		m_explosionOn = checkforCollision(m_bigPlaneLocation, m_bigPlaneRadius, 
										m_smallPlaneLocation, m_smallPaneRadius);
	}
	else
	{
		animateExplosion();
	}
}

/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::White);
	m_window.draw(m_skySprite);
	if (m_displayMessage)
	{
		m_window.draw(m_welcomeMessage);
	}
	m_window.draw(m_bigPlaneSprite);
	m_window.draw(m_smallPlaneSprite);
	if (m_explosionOn)
	{
		m_window.draw(m_explosionSprite);
	}
	m_window.display();
}

/// <summary>
/// update planes and sprites to new locations each frame
/// </summary>
void Game::movePlanes()
{
	m_bigPlaneLocation += m_bigPlaneVelocity;
	m_bigPlaneSprite.setPosition(m_bigPlaneLocation);
	m_smallPlaneLocation += m_smallPlaneVelocity;
	m_smallPlaneSprite.setPosition(m_smallPlaneLocation);
}
/// <summary>
/// if position outside of screen return to edge coordinate
/// </summary>
/// <param name="t_location">position</param>
void Game::keepOnScreen(sf::Vector2f &t_location)
{
	float screenWidth = static_cast<float>(WIDTH); // were ints, use floats for checks
	float screenHeight = static_cast<float>(HEIGHT);
	if (t_location.x < 0.0f)
	{
		t_location.x = 0.0f;
	}
	if (t_location.x > screenWidth)
	{
		t_location.x = screenWidth;
	}
	if (t_location.y < 0.0f)
	{
		t_location.y = 0.0f;
	}
	if (t_location.y > screenHeight)
	{
		t_location.y = screenHeight;
	}
}

/// <summary>
/// check if two vectors are close together
/// set location of explosion to be centre point between two
/// </summary>
/// <param name="t_locationOne">first centre</param>
/// <param name="t_radiusOne">first radius </param>
/// <param name="t_locationTwo">second centre point</param>
/// <param name="t_radiusTwo">seconf radius </param>
/// <returns></returns>
bool Game::checkforCollision(sf::Vector2f t_locationOne, float t_radiusOne, sf::Vector2f t_locationTwo, float t_radiusTwo)
{
	float distance = 0.0f;
	sf::Vector2f gap = t_locationOne - t_locationTwo;
	distance = vector2fLenght(gap);
	if (distance < (t_radiusOne + t_radiusTwo))
	{
		m_explosionSprite.setPosition((t_locationTwo + t_locationOne) /2.0f );
		return true;
	}
	return false;
}

/// <summary>
/// add time increment to framecounter and 
// change frame when we pass new int value
// only 48 frames in anumatuion then set to flase
/// </summary>
void Game::animateExplosion()
{
	m_explosionTimer += m_explosionIncrement;
	int frame = static_cast<int>(m_explosionTimer);
	if (frame != m_explosionframe)
	{
		m_explosionSprite.setTextureRect(sf::IntRect((frame % 8) * 100, (frame / 8) * 100, 100, 100));
		m_explosionframe = frame;
	}
	if (frame > 47)
	{
		frame = 0;
		m_explosionOn = false;
		m_explosionTimer = 0.0f;
	}
}

/// <summary>
/// load the font and setup the text message for screen
/// </summary>
void Game::setupFontAndText()
{
	if (!m_ArialBlackfont.loadFromFile("ASSETS\\FONTS\\ariblk.ttf"))
	{
		std::cout << "problem loading arial black font" << std::endl;
	}
	m_welcomeMessage.setFont(m_ArialBlackfont);
	m_welcomeMessage.setString("Drag left mouse button to control big \nairplane Right for small aeroplane\nPress any key to to clear this message ");

	m_welcomeMessage.setPosition(40.0f, 40.0f);
	m_welcomeMessage.setCharacterSize(40U);
	m_welcomeMessage.setFillColor(sf::Color::Black);


}

/// <summary>
/// load the texture and setup the sprites for the game
/// </summary>
void Game::setupSprites()
{
	sf::IntRect bigPlaneRect{ 3, 11, 104, 93 };
	sf::Vector2f bigPlaneStart{ 300.0f, 180.0f };
	sf::Vector2f smallPlaneStart{ 100.0f, 100.0f };
	sf::IntRect smallPlaneRect{ 362,115,87,69 };

	if (!m_AreoplaneTexture.loadFromFile("ASSETS\\IMAGES\\planes.png"))
	{
		// simple error message if previous call fails
		std::cout << "problem loading areoplanes" << std::endl;
	}
	m_bigPlaneSprite.setTexture(m_AreoplaneTexture);
	m_bigPlaneSprite.setTextureRect(bigPlaneRect);
	m_bigPlaneSprite.setPosition(bigPlaneStart);
	m_bigPlaneLocation = bigPlaneStart;
	// we will be rotating sprite so best to have origin at centre
	m_bigPlaneSprite.setOrigin(bigPlaneRect.width / 2.0f, bigPlaneRect.height / 2.0f);
	m_bigPlaneSprite.setRotation(m_bigPlaneHeading);


	m_smallPlaneSprite.setTexture(m_AreoplaneTexture);
	m_smallPlaneSprite.setTextureRect(smallPlaneRect);
	m_smallPlaneSprite.setPosition(smallPlaneStart);
	m_smallPlaneLocation = smallPlaneStart;
	m_smallPlaneSprite.setOrigin(smallPlaneRect.width / 2.0f, smallPlaneRect.height / 2.0f);
	m_smallPlaneSprite.setRotation(m_smallPlaneHeading);


	if (!m_skyTexture.loadFromFile("ASSETS\\IMAGES\\sky.jpg"))
	{
		std::cout << "problem loading sky" << std::endl;
	}
	m_skyTexture.setRepeated(true);
	m_skySprite.setTexture(m_skyTexture);
	m_skySprite.setTextureRect(sf::IntRect(0, 0, WIDTH, HEIGHT));

	if (!m_skyTexture.loadFromFile("ASSETS\\IMAGES\\sky.jpg"))
	{
		std::cout << "problem loading sky" << std::endl;
	}
	m_skyTexture.setRepeated(true);
	
	if (!m_explosionTexture.loadFromFile("ASSETS\\IMAGES\\explosion.png"))
	{
		std::cout << "problem loading explosion" << std::endl;
	}
	m_explosionSprite.setTexture(m_explosionTexture);
	m_explosionSprite.setOrigin(50.0f, 50.0f);
	m_explosionSprite.setTextureRect(sf::IntRect{ 0, 0, 100, 100 });
}


/// <summary>
/// helper method to calculate the lenght od vextor
/// </summary>
/// <param name="t_vector">vector</param>
/// <returns>lenght</returns>
float vector2fLenght(sf::Vector2f t_vector)
{
	float result = std::sqrtf(t_vector.x * t_vector.x + t_vector.y * t_vector.y);
	return result;
}
