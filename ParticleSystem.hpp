#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <thread>
#include <chrono>
#include "Particle.hpp"
#include "QuadTreeNode.hpp"

class ParticleSystem
{
public:
	ParticleSystem(int num_particles, int num_threads, bool use_bh, int window_width, int window_height);
	ParticleSystem(int num_particles, int num_threads, bool use_bh);
	void update();
	std::vector<Particle> particles[2]; // Double buffer

	int current_buffer = 0;
	int wh;
	int ww;

private:
	void init_system();
	void build_tree();
	void update_single_threaded();
	void update_multi_threaded();
	void update_particle(int index, const std::vector<Particle> &source, std::vector<Particle> &target);
	void update_particle_pair(Particle &p1, const Particle &p2, std::array<float, 3> &p2_vel);
	void update_position(Particle &p);

	int num_particles;
	int num_threads;
	bool is_rendered;
	bool use_bh;
	QuadTreeNode bh_tree;
};
