#include "ParticleSystem.hpp"

// Constructor for rendering the system
ParticleSystem::ParticleSystem(int num_particles, int num_threads, bool is_rendered, int window_width, int window_height)
	: num_particles{num_particles},
	  num_threads{num_threads},
	  is_rendered{is_rendered},
	  ww{window_width},
	  wh{window_height}
{
	init_system();
}

// Constructor for benchmarking
ParticleSystem::ParticleSystem(int num_particles, int num_threads, bool is_rendered)
	: num_particles{num_particles},
	  num_threads{num_threads},
	  is_rendered{is_rendered},
	  ww{},
	  wh{}
{
	init_system();
}

void ParticleSystem::init_system()
{
	particles[0].resize(num_particles);
	particles[1].resize(num_particles);
	int large_mass_index;

	// Set a random particle to be much larger mass just for visualization purposes when rendered
	if (is_rendered)
	{
		large_mass_index = (rand() % num_particles) + 1;
	}

	for (int i = 0; i < num_particles; i++)
	{
		particles[0][i] = {
			// x, y, z
			static_cast<float>(rand() % ww),
			static_cast<float>(rand() % wh),
			static_cast<float>(rand() % 1000),
			// mass
			(is_rendered && large_mass_index == i) ? 20.0f : 1.0f,
			// vx, vy, vz
			0.0f,
			0.0f,
			0.0f,
			is_rendered ? sf::Color(rand() % 256, rand() % 256, rand() % 256) : sf::Color()};
		particles[1][i] = particles[0][i];
	}
}

void ParticleSystem::update()
{
	if (num_threads == 1)
	{
		update_single_threaded();
	}
	else
	{
		update_multi_threaded();
	}
}

void ParticleSystem::update_single_threaded()
{
	for (int i = 0; i < num_particles; i++)
	{
		for (int j = i + 1; j < num_particles; j++)
		{
			update_particle_pair(particles[current_buffer][i], particles[current_buffer][j]);
		}
	}
	update_positions();
}

void ParticleSystem::update_multi_threaded()
{
	int source_buffer = current_buffer;
	int target_buffer = 1 - current_buffer;

	std::vector<std::thread> threads;
	int particles_per_thread = num_particles / num_threads;

	for (int t = 0; t < num_threads; ++t)
	{
		int start = t * particles_per_thread;
		int end = (t == num_threads - 1) ? num_particles : (t + 1) * particles_per_thread;

		threads.emplace_back([this, start, end, source_buffer, target_buffer]()
							 {
                for (int i = start; i < end; ++i) {
                    update_particle(i, particles[source_buffer], particles[target_buffer]);
                } });
	}

	for (auto &thread : threads)
	{
		thread.join();
	}

	current_buffer = target_buffer;
	update_positions();
}

void ParticleSystem::update_particle(int index, const std::vector<Particle> &source, std::vector<Particle> &target)
{
	Particle &p = target[index];
	p = source[index]; // Start with the current state

	for (int j = 0; j < num_particles; ++j)
	{
		if (j == index)
			continue;
		const Particle &other = source[j];

		float dx = other.x - p.x;
		float dy = other.y - p.y;
		float dz = other.z - p.z;
		float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

		if (dist < 1e-10)
			continue; // Avoid division by zero

		float force = p.mass * other.mass / (dist * dist * dist);
		p.vx += force * dx;
		p.vy += force * dy;
		p.vz += force * dz;
	}
	p.x += p.vx;
	p.y += p.vy;
	p.z += p.vz;
}

void ParticleSystem::update_particle_pair(Particle &p1, Particle &p2)
{
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	float dz = p2.z - p1.z;
	float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
	float force = p1.mass * p2.mass / (dist * dist * dist);
	p1.vx += force * dx;
	p1.vy += force * dy;
	p1.vz += force * dz;
	p2.vx -= force * dx;
	p2.vy -= force * dy;
	p2.vz -= force * dz;
}

void ParticleSystem::update_positions()
{
	for (auto &p : particles[current_buffer])
	{
		p.x += p.vx;
		p.y += p.vy;

		// Only constrain x,y coordinates to window dimensions if rendering particle motion
		if (is_rendered)
		{
			if (p.x <= 0 || p.x >= ww)
			{
				p.vx = -p.vx;
				p.x = std::max(0.0f, std::min(p.x, static_cast<float>(ww)));
			}
			if (p.y <= 0 || p.y >= wh)
			{
				p.vy = -p.vy;
				p.y = std::max(0.0f, std::min(p.y, static_cast<float>(wh)));
			}
		}

		p.z += p.vz;
	}
}