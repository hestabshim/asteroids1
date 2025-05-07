
#include <iostream>
#include <SFPhysics.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <chrono>
#include <random>
using namespace std;
using namespace sfp;
using namespace sf;


void LoadTex(Texture& tex, string filename) {
	if (!tex.loadFromFile(filename)) {
		cout << "Could not load " << filename << endl;
	}
}

//random number generator
float randf(float min, float max) {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}
int numast = 0;
//spawn asteroid function
void newAsteroid(int& score, PhysicsSprite& ship, Vector2f spawn_pos, float size_mult, map<PhysicsSprite*, float>& asteroidSizes, PhysicsShapeList<PhysicsSprite>& asteroids, World& world, PhysicsCircle& bullet, Texture& astTex, Vector2u& windowSize) {

	//PhysicsSprite& asteroid = *new PhysicsSprite();
	if (numast > 26) {
		return;
	}
	PhysicsSprite& asteroid = asteroids.Create();
	numast++;
	asteroidSizes[&asteroid] = size_mult;
	
	
	//asteroid collision with bullet
	asteroid.onCollision = [&](PhysicsBodyCollisionResult result) {
		float currSize = asteroidSizes[&asteroid];
		//cout << "curr size " << currSize << endl;

		//cout << "ASTEROID HIT SOMETHING" << result.hasCollided << endl;
		if (result.object2 == bullet) {
			world.RemovePhysicsBody(asteroid);
			world.RemovePhysicsBody(bullet);
			
			//asteroids.erase(asteroids.begin() + myIdx);
			//asteroids[myIdx].second = 0;
			asteroids.QueueRemove(asteroid);
			score += 50 + 1.0 / currSize;
			numast--;
			



			if (currSize > 0.1) {
				float newSize = currSize / 2.0f;
				//cout << "before" << endl;
				Vector2f spawn_pos = asteroid.getCenter();
				newAsteroid(score, ship, spawn_pos, newSize, asteroidSizes, asteroids, world, bullet, astTex, windowSize);
				newAsteroid(score, ship, spawn_pos, newSize, asteroidSizes, asteroids, world, bullet, astTex, windowSize);
				//cout << "after" << endl;
				
			}

			

		}
		};

	//randomized spwaning
	asteroid.setTexture(astTex);
	Vector2f sz = asteroid.getSize();
	float magic = 1.0 / (2 * size_mult) - 0.5;

	FloatRect size = asteroid.getGlobalBounds();
	//((PhysicsBody&)asteroid).setPosition(Vector2f(randf(0, windowSize.x), randf(0, windowSize.y)));
	asteroid.setCenter(spawn_pos);

	asteroid.setOrigin(-size.width * magic, -size.height * magic);

	asteroid.setVelocity(Vector2f(randf(-0.25, 0.25), randf(-0.25, 0.25)) * (0.1f / size_mult));
	asteroid.setSize(Vector2f(sz * size_mult));

	world.AddPhysicsBody(asteroid);
	asteroid.setMass(0);



}

int main()
{
	numast = 0;
	RenderWindow window(VideoMode(1200, 900), "Asteroids");
	World world(Vector2f(0, 0));
	int score(0);

	//background
	Sprite background;
	Texture backgroundTex;
	LoadTex(backgroundTex, "images/background.jpg"); 
	background.setTexture(backgroundTex); 
	background.setOrigin(600, 450);
	background.setScale(2,2);

	//ship
	PhysicsSprite ship;
	Texture shipTex;
	LoadTex(shipTex, "images/ship.png");
	ship.setTexture(shipTex);
	Vector2f sz = ship.getSize();
	ship.setCenter(Vector2f(600, 450));
	ship.setOrigin(15, 7.5);
	ship.rotate(1);
	

	//bullet
	PhysicsCircle bullet;
	bullet.setSize(Vector2f(5, 5));
	bool drawbullet(false);

	
	PhysicsShapeList<PhysicsSprite> asteroids;
	Texture astTex;
	LoadTex(astTex, "images/asteroid.png");

	map<PhysicsSprite*, float> asteroidSizes;
	
	Vector2u windowSize = window.getSize();
	
	background.setPosition(windowSize.x / 2, windowSize.y / 2); 

	//text
	Font font;
	Text gameOver;
	Text playAgain;
	Text scoreText;
	Text titleCard;

	font.loadFromFile("fonts/dogicapixelbold.ttf");

	gameOver.setFont(font);
	gameOver.setString("GAME OVER!");
	gameOver.setCharacterSize(100);

	playAgain.setString("PRESS R TO PLAY AGAIN");
	playAgain.setCharacterSize(25);
	playAgain.setFont(font);

	titleCard.setString("ASTEROIDS");
	titleCard.setFont(font);
	titleCard.setCharacterSize(75);
	
	FloatRect header = titleCard.getLocalBounds();
	titleCard.setOrigin(header.width / 2, header.height/2 + 200.f);
	titleCard.setPosition(Vector2f(windowSize) * 0.5f);
	
	scoreText.setCharacterSize(25);
	scoreText.setFont(font);
	FloatRect scoreRect = scoreText.getLocalBounds();
	scoreText.setOrigin(scoreRect.width / 2, scoreRect.height / 2);

	FloatRect text = gameOver.getLocalBounds();
	gameOver.setOrigin(text.width / 2, text.height / 2);
	gameOver.setPosition(windowSize.x / 2, windowSize.y / 2);

	FloatRect text2 = playAgain.getLocalBounds();
	playAgain.setOrigin(text2.width / 2, text2.height / 2);
	playAgain.setPosition(windowSize.x / 2, windowSize.y / 2 + 100.f);
	


	//clock
	Clock clock;
	Time lastTime = clock.getElapsedTime();
	Time lastBullet = clock.getElapsedTime();
	Time lastAsteroid = clock.getElapsedTime();
	Time textTime = clock.getElapsedTime();
	//game
	while (window.isOpen()) {
		
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();

		}
		//world time

		Time currentTime = clock.getElapsedTime();
		int deltaMS = (currentTime - lastTime).asMilliseconds();

		//bullet time
		int bulletMS = (currentTime - lastBullet).asMilliseconds();
		int deltaAsteroidMS = (currentTime - lastAsteroid).asMilliseconds();

		//ship movement speed
		float speed = 0.03f;
		float radians = ship.getRotation() * 3.14159265358979323846f / 180.0f;
		float dx = cos(radians) * speed;
		float dy = sin(radians) * speed;

		Vector2f movement(dx, dy);
		Vector2f backmovement(-dx, -dy);
		int textMS = (currentTime - textTime).asMilliseconds();
		//cout << textMS << endl;
	
		//world update
		if (deltaMS > 9) {
			
			window.draw(background);
			Vector2f positionShip = ship.getCenter();

			float antigravity = 0.5;

			for (auto& asteroid : asteroids) {
				Vector2f positionAsteroid = asteroid.getCenter();
				positionAsteroid.x = fmod(positionAsteroid.x + windowSize.x, windowSize.x);
				positionAsteroid.y = fmod(positionAsteroid.y + windowSize.y, windowSize.y) - antigravity;
				asteroid.setCenter(positionAsteroid);
			}

			positionShip.x = fmod(positionShip.x + windowSize.x, windowSize.x);
			positionShip.y = fmod(positionShip.y + windowSize.y, windowSize.y) - antigravity;
			
			ship.setCenter(positionShip);
			
		
			//movement
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				ship.move(movement * 150.f);
				//cout << "works " << movement.x << " " << movement.y << endl;
			}
			if (Keyboard::isKeyPressed(Keyboard::S)) {
				ship.move(backmovement * 150.f);
			}
			if (Keyboard::isKeyPressed(Keyboard::A)) {
				ship.rotate(5.f);
			}
			if (Keyboard::isKeyPressed(Keyboard::D)) {
				ship.rotate(-5.f);
			}
			if (Keyboard::isKeyPressed(Keyboard::Space) && !drawbullet) {
				drawbullet = true;
				world.AddPhysicsBody(bullet);
				bullet.setCenter(ship.getCenter());
				bullet.setVelocity(movement * 75.f);
				bullet.setMass(0);
			}
			if (bulletMS > 250) {
				lastBullet = currentTime;
				drawbullet = false;
				world.RemovePhysicsBody(bullet);
				//clock.restart();
			}
			lastTime = currentTime;
			
			

			world.UpdatePhysics(deltaMS);
			window.clear();
			//window.draw(background);
			//asteroid collision w/ship
			for (auto& asteroid : asteroids) {

				auto collision = asteroid.getGlobalBounds();
				//auto shipCollision = ship.getGlobalBounds();
				if (collision.contains(ship.getCenter())) {
					//cout << "collid pls" << endl;
					window.clear();
					scoreText.setPosition(windowSize.x / 2, windowSize.y / 2 + 150.0f);
					window.draw(gameOver);
					window.draw(playAgain);
					window.draw(scoreText);
					window.display();
					while (true) {
						while (window.pollEvent(event)) {
							if (event.type == Event::Closed) { window.close(); }
							
						}
						if (Keyboard::isKeyPressed(Keyboard::R)) {
							main();
						}
					}
				}
				
				window.draw((PhysicsSprite&)asteroid);
			}
			
			if (textMS < 2500) {
				//cout << "i" << textMS << endl;
				window.draw(titleCard);
			}
			scoreText.setString(to_string(score));
			window.draw(scoreText);
			if (drawbullet) window.draw(bullet);
			window.draw(ship); 
			//world.VisualizeAllBounds(window);
			window.display();

			asteroids.DoRemovals();
		}

		//cerr << "WHAT THE FUCJK\n";
		//spawn asteroids
		if (deltaAsteroidMS > 1000) {
			lastAsteroid = currentTime;

			float size_mult = randf(0.1, 0.5);
			Vector2f spawn_pos = Vector2f(randf(0, windowSize.x), randf(0, windowSize.y));
			auto distsq = [&](Vector2f a, Vector2f b) {
				float dx = a.x - b.x;
				float dy = a.y - b.y;
				return dx * dx + dy * dy;
			};
			float dist = 200;
			while (distsq(spawn_pos, ship.getCenter()) < dist*dist) {
				cout << "YOU GOT SAVED BUDDY\n";
				spawn_pos = Vector2f(randf(0, windowSize.x), randf(0, windowSize.y));
			}
			newAsteroid(score, ship, spawn_pos, size_mult, asteroidSizes, asteroids, world, bullet, astTex, windowSize);
		}
	}
}


