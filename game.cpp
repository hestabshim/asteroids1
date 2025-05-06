
#include <iostream>
#include <SFPhysics.h>
#include <SFML/Graphics.hpp>
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
float randf(float min, float max) {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}
void newAsteroid(Vector2f spawn_pos, float size_mult, map<PhysicsSprite*, float>& asteroidSizes, PhysicsShapeList<PhysicsSprite>& asteroids, World& world, PhysicsCircle& bullet, Texture& astTex, Vector2u& windowSize) {

	//PhysicsSprite& asteroid = *new PhysicsSprite();
	PhysicsSprite& asteroid = asteroids.Create();
	asteroidSizes[&asteroid] = size_mult;
	//int myIdx = asteroids.size();
	//cout << "hi" << size_mult << endl;

	//asteroids.push_back({ asteroid, size_mult });
	asteroid.onCollision = [&](PhysicsBodyCollisionResult result) {
		float currSize = asteroidSizes[&asteroid];
		cout << "curr size " << currSize << endl;

		if (result.object2 == bullet) {
			world.RemovePhysicsBody(asteroid);
			world.RemovePhysicsBody(bullet);
			//asteroids.erase(asteroids.begin() + myIdx);
			//asteroids[myIdx].second = 0;
			asteroids.QueueRemove(asteroid);
			

			if (currSize > 0.1) {
				float newSize = currSize / 2.0f;
				cout << "before" << endl;
				Vector2f spawn_pos = asteroid.getCenter();
				newAsteroid(spawn_pos, newSize, asteroidSizes, asteroids, world, bullet, astTex, windowSize);
				newAsteroid(spawn_pos, newSize, asteroidSizes, asteroids, world, bullet, astTex, windowSize);
				cout << "after" << endl;
			}



			cerr << "collide\n";
		}
		};

	//randomized spwaning
	asteroid.setTexture(astTex);
	Vector2f sz = asteroid.getSize();
	//float size_mult = 0.125/2.0;
	float magic = 1.0 / (2 * size_mult) - 0.5;

	FloatRect size = asteroid.getGlobalBounds();
	//((PhysicsBody&)asteroid).setPosition(Vector2f(randf(0, windowSize.x), randf(0, windowSize.y)));
	asteroid.setCenter(spawn_pos);

	asteroid.setOrigin(-size.width * magic, -size.height * magic);
	//asteroid.setOrigin(0, 0);
	//asteroid.setOrigin(0, 0);
	asteroid.setVelocity(Vector2f(randf(-0.25, 0.25), randf(-0.25, 0.25)) * (0.1f / size_mult));
	//asteroid.setScale(Vector2f(size_mult, size_mult));
	asteroid.setSize(Vector2f(sz * size_mult));
	//asteroid.setRotation(randf(0, 360));

	world.AddPhysicsBody(asteroid);
	asteroid.setMass(0);



}

int main()
{
	RenderWindow window(VideoMode(1200, 900), "Asteroids");
	World world(Vector2f(0, 0));

	//ship
	PhysicsSprite ship;
	Texture shipTex;
	LoadTex(shipTex, "images/ship.png");
	ship.setTexture(shipTex);
	Vector2f sz = ship.getSize();
	ship.setCenter(Vector2f(600, 450));
	ship.setOrigin(15, 7.5);

	//bullet
	PhysicsCircle bullet;
	bullet.setSize(Vector2f(5, 5));
	bool drawbullet(false);

	//PhysicsSprite& asteroid = *new PhysicsSprite();
	//vector<pair<PhysicsSprite&, float>> asteroids;
	PhysicsShapeList<PhysicsSprite> asteroids;
	Texture astTex;
	LoadTex(astTex, "images/asteroid.png");

	map<PhysicsSprite*, float> asteroidSizes;


	Vector2u windowSize = window.getSize();



	//clock
	Clock clock;
	Time lastTime = clock.getElapsedTime();
	Time lastBullet = clock.getElapsedTime();
	Time lastAsteroid = clock.getElapsedTime();

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

		float speed = 0.03f;
		float radians = ship.getRotation() * 3.14159265358979323846f / 180.0f;
		float dx = cos(radians) * speed;
		float dy = sin(radians) * speed;

		Vector2f movement(dx, dy);
		Vector2f backmovement(-dx, -dy);

		if (deltaMS > 9) {
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				ship.move(movement * 150.f);
				cout << "works " << movement.x << " " << movement.y << endl;
			}
			if (Keyboard::isKeyPressed(Keyboard::S)) {
				ship.move(backmovement * 150.f);
			}
			if (Keyboard::isKeyPressed(Keyboard::A)) {
				ship.rotate(3.f);
			}
			if (Keyboard::isKeyPressed(Keyboard::D)) {
				ship.rotate(-3.f);
			}
			if (Keyboard::isKeyPressed(Keyboard::Space) && !drawbullet) {
				drawbullet = true;
				world.AddPhysicsBody(bullet);
				bullet.setCenter(ship.getCenter());
				bullet.setVelocity(movement * 50.f);
				bullet.setMass(0);
			}
			if (bulletMS > 500) {
				lastBullet = currentTime;
				drawbullet = false;
				world.RemovePhysicsBody(bullet);
				//clock.restart();
			}
			lastTime = currentTime;

			world.UpdatePhysics(deltaMS);
			window.clear();
			//for (auto& pair : asteroids) {
			//	if (pair.second == 0) continue;
			//	/*float astrot = asteroid.getRotation();
			//	asteroid.setRotation(astrot + randf(0.1,1));*/
			//	window.draw((PhysicsSprite&) pair.first);
			//}
			for (auto& asteroid : asteroids) {

				//float astrot = asteroid.getRotation();
				//asteroid.setRotation(astrot + randf(0.1,1));
				window.draw((PhysicsSprite&)asteroid);
			}
			window.draw(bullet);
			//window.draw(asteroid);
			window.draw(ship);
			/*window.draw(right);
			window.draw(left);
			window.draw(top);
			window.draw(bottom);*/
			//window.draw(asteroid);
			world.VisualizeAllBounds(window);
			window.display();

			asteroids.DoRemovals();
		}

		//cerr << "WHAT THE FUCJK\n";
		if (deltaAsteroidMS > 2000) {
			lastAsteroid = currentTime;

			float size_mult = randf(0.1, 0.5);
			newAsteroid(Vector2f(randf(0, windowSize.x), randf(0, windowSize.y)), size_mult, asteroidSizes, asteroids, world, bullet, astTex, windowSize);
		}

	}
}


