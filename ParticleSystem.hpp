#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>

struct Particle
{
	float x, y, z;
	float mass;
	float vx, vy, vz;
	sf::Color color;
};

class ParticleSystem
{
public:
	ParticleSystem(int num_particles, int num_threads, bool is_rendered, int window_width, int window_height);
	ParticleSystem(int num_particles, int num_threads, bool is_rendered);
	void update();
	std::vector<Particle> particles[2]; // Double buffer

	int current_buffer = 0;
	int wh;
	int ww;

private:
	void init_system();
	void update_single_threaded();
	void update_multi_threaded();
	void update_particle(int index, const std::vector<Particle> &source, std::vector<Particle> &target);
	void update_particle_pair(Particle &p1, Particle &p2);
	void update_positions();

	int num_particles;
	int num_threads;
	bool is_rendered;
};
