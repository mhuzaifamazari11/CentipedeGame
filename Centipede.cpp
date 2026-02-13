#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
using namespace sf;
using namespace std;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid
const float playerSpeed = 0.1f; // player speed in movement
const int numMushrooms = 25;


// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

const int x = 0;
const int y = 1;
const int exists = 2;


// Function to draw the player on the game window
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite);

// Function to move the bullet
void moveBullet(float bullet[], sf::Clock& bulletClock);

// Function to draw the bullet on the game window
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite);

// Fire Bullet
void fireBullet(float bullet[], float player[], sf::Sound& bulletSound);

// Function to initialize random mushrooms on the game grid
void initializeMushrooms(int gameGrid[][gameColumns], int numMushrooms);

// Function to check if a mushroom is too close to other mushrooms
bool isTooCloseToOtherMushrooms(int x, int y, int gameGrid[][gameColumns], int minDistance);

// Function to draw mushrooms on the game window
void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite, int gameGrid[][gameColumns]);

// Function to handle shooting mechanics
void handleShooting(float bullet[], float player[]);

// Function to handle player movement
void handlePlayerMovement(float player[]);

// Function to move the centipede
void moveCentipede(float centipedeX[], float centipedeY[], int centipedeDirection[], int numSegments, int gameGrid[][gameColumns]);

// Function to draw the centipede on the game window
void drawCentipede(sf::RenderWindow& window, sf::Texture& headTexture, sf::Texture& bodyTexture, float centipedeX[], float centipedeY[], int numSegments);


int main()
{
	srand(time(0));

	// Declaring RenderWindow.
	sf::RenderWindow window(sf::VideoMode(resolutionX, resolutionY), "C e n t i p e d e", sf::Style::Close | sf::Style::Titlebar);


	window.setSize(sf::Vector2u(640, 640)); 

	window.setPosition(sf::Vector2i(100, 200));

	//Background Music.
	sf::Music bgMusic;
	bgMusic.openFromFile("Music/night_city.ogg");
	bgMusic.play();
	bgMusic.setVolume(50);

	// Initializing Background.
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/background_r.png");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.50)); // Reduces Opacity to 25%

	// Initializing Player and Player Sprites.
	float player[2] = {};
	player[x] = (gameColumns / 2) * boxPixelsX;
	player[y] = (gameRows * 85/100) * boxPixelsY;
	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// Initializing Bullet and Bullet Sprites.
	float bullet[3] = {};
	bullet[x] = player[x];
	bullet[y] = player[y] - boxPixelsY;
	bullet[exists] = true;
	sf::Clock bulletClock;
	sf::Texture bulletTexture;
	sf::Sprite bulletSprite;
	bulletTexture.loadFromFile("Textures/bullet.png");
	bulletSprite.setTexture(bulletTexture);
	bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	//Bullet Sound
	sf::SoundBuffer bulletSoundBuffer;
	bulletSoundBuffer.loadFromFile("Sound Effects/fire1.wav");
	sf::Sound bulletSound;
	bulletSound.setBuffer(bulletSoundBuffer);
	
	
	// Initializing Mushroom and Mushroom Sprites.
	int mushroomGrid[gameRows][gameColumns] = {};
	sf::Texture mushroomTexture;
	sf::Sprite mushroomSprite;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	initializeMushrooms(mushroomGrid, numMushrooms);
	
	
	
	// Number of segments in the centipede
	const int numCentipedeSegments = 12;

	// Arrays to store centipede information
	float centipedeX[numCentipedeSegments];
	float centipedeY[numCentipedeSegments];
	int centipedeDirection[numCentipedeSegments];

	// Initialize centipede with random x position in row 1
	int initialX = rand() % (gameRows - numCentipedeSegments) * boxPixelsX;  // Random x position in row 1

	// Loop to set initial values for each segment of the centipede
	for (int i = 0; i < numCentipedeSegments; ++i) {
		centipedeX[i] = initialX + i * boxPixelsX;  // Spread the centipede segments horizontally
		centipedeY[i] = 0;               // Initial y position
		centipedeDirection[i] = -1;       // Initial direction (left)
	}

	// Texture for the head of the centipede
	sf::Texture headTexture;
	headTexture.loadFromFile("Textures/c_head_left_walk.png");  

	// Texture for the body of the centipede
	sf::Texture bodyTexture;
	bodyTexture.loadFromFile("Textures/c_body_left_walk.png");  

	// Game loop
	while (window.isOpen()) {
	    sf::Event e;
	    while (window.pollEvent(e)) {
		if (e.type == sf::Event::Closed) {
		    window.close();
		}
	    }

	    // Check for firing bullet
	    if (!bullet[exists] && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		fireBullet(bullet, player, bulletSound);
	    }

	    // Move the bullet
	    moveBullet(bullet, bulletClock);

	    // Handle player movement
	    handlePlayerMovement(player);

	    // Move the centipede
	    moveCentipede(centipedeX, centipedeY, centipedeDirection, numCentipedeSegments, mushroomGrid);

	    // Clear the window
	    window.clear();

	    // Draw background
	    window.draw(backgroundSprite);

	    // Draw player
	    drawPlayer(window, player, playerSprite);

	    // Draw mushrooms
	    drawMushrooms(window, mushroomSprite, mushroomGrid);

	    // Draw bullet
	    drawBullet(window, bullet, bulletSprite);

	    // Draw centipede
	    drawCentipede(window, headTexture, bodyTexture, centipedeX, centipedeY, numCentipedeSegments);

	    // Display the updated window
	    window.display();
	}
}

void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite) {
	playerSprite.setPosition(player[x], player[y]);
	window.draw(playerSprite);
}
void moveBullet(float bullet[], sf::Clock& bulletClock) {
    if (bulletClock.getElapsedTime().asMilliseconds() < 15)
        return;

    bulletClock.restart();
    bullet[y] -= 10;

    if (bullet[y] < -32)
        bullet[exists] = false;
}
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite) {
	bulletSprite.setPosition(bullet[x], bullet[y]);
	window.draw(bulletSprite);
}
void fireBullet(float bullet[], float player[], sf::Sound& bulletSound) {
    bullet[x] = player[x];
    bullet[y] = player[y] - boxPixelsY;
    bullet[exists] = true;

    // Play the bullet sound
    bulletSound.play();
}
void handleShooting(float bullet[], float player[]) {
    bullet[x] = player[x];
    bullet[y] = player[y] - boxPixelsY;
}
void initializeMushrooms(int gameGrid[][gameColumns], int numMushrooms) {
    const int minDistance = 1;  // Minimum distance between mushrooms

    for (int i = 0; i < numMushrooms; ++i) {
        int randomX, randomY;

        do {
             randomX = rand() % (gameRows - 1) + 1;  // Exclude the first column and first row
             randomY = rand() % (gameColumns - 7) + 1;  // Exclude the last 8 rows and last column
        } while (gameGrid[randomX][randomY] == exists || isTooCloseToOtherMushrooms(randomX, randomY, gameGrid, minDistance));

        // Place a mushroom at the random position
        gameGrid[randomX][randomY] = exists;
    }
}

bool isTooCloseToOtherMushrooms(int x, int y, int gameGrid[][gameColumns], int minDistance) {
    for (int i = std::max(0, x - minDistance); i <= std::min(gameRows - 1, x + minDistance); ++i) {
        for (int j = std::max(0, y - minDistance); j <= std::min(gameColumns - 1, y + minDistance); ++j) {
            if (gameGrid[i][j] == exists) {
                return true;  // There's a mushroom too close
            }
        }
    }
    return false;  // No mushroom too close
}






void drawMushrooms(sf::RenderWindow& window, sf::Sprite& mushroomSprite, int gameGrid[][gameColumns]) {
    for (int i = 0; i < gameRows; ++i) {
        for (int j = 0; j < gameColumns; ++j) {
            if (gameGrid[i][j] == exists) {
                mushroomSprite.setPosition(i * boxPixelsX, j * boxPixelsY);
                window.draw(mushroomSprite);
            }
        }
    }
}
void handlePlayerMovement(float player[]) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player[x] -= playerSpeed;
        if (player[x] < 0) {
            player[x] = 0;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player[x] += playerSpeed;
        if (player[x] > resolutionX - boxPixelsX) {
            player[x] = resolutionX - boxPixelsX;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        // Move up only if the player is in the last 5 rows
        if (player[y] > resolutionY - boxPixelsY * 5) {
            player[y] -= playerSpeed;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        // Move down only if the player is in the last 5 rows
        if (player[y] < resolutionY - boxPixelsY) {
            player[y] += playerSpeed;
        }
    }
}
void moveCentipede(float centipedeX[], float centipedeY[], int centipedeDirection[], int numSegments, int gameGrid[][gameColumns]) {
    for (int i = 0; i < numSegments; ++i) {
        centipedeX[i] += 0.08f * centipedeDirection[i];
        bool HitOnMushroom = false;  // Flag to check if the centipede hits a mushroom

        // Reverse direction if the centipede hits the window boundaries
        if (centipedeX[i] < 0 || centipedeX[i] > resolutionX - boxPixelsX) {
            centipedeDirection[i] *= -1;
            centipedeY[i] += boxPixelsY;  // Move down when reversing direction     
        }

        int centipedeGridX = centipedeX[i] / boxPixelsX;
        int centipedeGridY = centipedeY[i] / boxPixelsY;

        // Bounds checking to avoid accessing outside the array
        if (centipedeGridX > 0 && centipedeGridX < gameRows && centipedeGridY >= 0 && centipedeGridY < gameColumns) {
            if (gameGrid[centipedeGridX+1][centipedeGridY] == exists) {
                // Reverse the centipede's direction if it hits a mushroom
                centipedeY[i] += boxPixelsY;  // Move down when reversing direction
                centipedeDirection[i] *= -1;
                HitOnMushroom = true;  // Set the flag to true
            }
        }
    }
}




void drawCentipede(sf::RenderWindow& window, sf::Texture& headTexture, sf::Texture& bodyTexture, float centipedeX[], float centipedeY[], int numSegments) {
    for (int i = 0; i < numSegments; ++i) {
        sf::Sprite centipedeSprite;

        // Use different textures for the head and body segments
        if (i == 0) {
            centipedeSprite.setTexture(headTexture);
            centipedeSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY)); // Adjust the texture rectangle as needed
        } else {
            centipedeSprite.setTexture(bodyTexture);
            centipedeSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY)); // Adjust the texture rectangle as needed
        }

        centipedeSprite.setPosition(centipedeX[i], centipedeY[i]);
        window.draw(centipedeSprite);
    }
}



















