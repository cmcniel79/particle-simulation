#ifndef MY_HEADER_FILE_
#define MY_HEADER_FILE_
#include <SFML/Graphics.hpp>

struct Particle
{
	float x, y, z;
	float mass;
	float vx, vy, vz;
	sf::Color color;
};

#endif