#include "ParticleSystem.hpp"

// Constructor for rendering the system
ParticleSystem::ParticleSystem(int num_particles, int num_threads, bool use_bh, int window_width, int window_height)
	: num_particles{num_particles},
	  num_threads{num_threads},
	  is_rendered{true},
	  ww{window_width},
	  wh{window_height},
	  use_bh{use_bh},
	  bh_tree{ 
		  window_width / 2.0f, 
		  window_height / 2.0f, 
		  static_cast<float>(std::max(window_width, window_height))
	  }
{
	init_system();
}

// Constructor for benchmarking
ParticleSystem::ParticleSystem(int num_particles, int num_threads, bool use_bh)
	: num_particles{num_particles},
	  num_threads{num_threads},
	  is_rendered{false},
	  ww{1000},
	  wh{1000},
	  use_bh{use_bh},
	  bh_tree{500.0f, 500.0f, 500.0f}
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
			(is_rendered && large_mass_index == i) ? 20.0f : 2.0f,
			// vx, vy, vz
			0.0f,
			0.0f,
			0.0f,
			// color
			is_rendered ? sf::Color(rand() % 256, rand() % 256, rand() % 256) : sf::Color()
		};

		// Copy the initial state to the second buffer
		particles[1][i] = particles[0][i];
	}
	if (use_bh) {
		build_tree();
	}
}

void ParticleSystem::build_tree()
{

}

// Wrapper function for the real update functions,
// calls either the single-threaded or multi-threaded version
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

// O(n^2) update function for the single-threaded version
void ParticleSystem::update_single_threaded()
{
	for (int i = 0; i < num_particles; i++)
	{
		for (int j = i + 1; j < num_particles; j++)
		{
			std::array<float, 3> p2_vel{0.0f, 0.0f, 0.0f};
			update_particle_pair(particles[current_buffer][i], particles[current_buffer][j], p2_vel);
			// Todo: Refactor so not so messy
			particles[current_buffer][j].vx = p2_vel[0];
			particles[current_buffer][j].vy = p2_vel[1];
			particles[current_buffer][j].vz = p2_vel[2];
		}
	}
	// Update the position of each particle after all forces have been calculated
	for (int i = 0; i < num_particles; i++)
	{
		update_position(particles[current_buffer][i]);
	}
}

void ParticleSystem::update_multi_threaded()
{
	int source_buffer = current_buffer;
	int target_buffer = 1 - current_buffer;

	std::vector<std::thread> threads;
	int particles_per_thread = num_particles / num_threads;

	// Distribute particles among threads
	for (int t = 0; t < num_threads; ++t)
	{
		int start = t * particles_per_thread;

		// End of the last thread will be the total number of particles,
		// for all other threads it will be the start of the next thread
		int end = (t == num_threads - 1) ? num_particles : (t + 1) * particles_per_thread;

		// Create a thread to update the particles in the range (start, end]
		// The lambda function captures the variables by reference
		threads.emplace_back([this, start, end, source_buffer, target_buffer]() 
		{
            for (int i = start; i < end; ++i) 
			{
                update_particle(i, particles[source_buffer], particles[target_buffer]);
            } 
		});
	}

	// Wait for all threads to finish
	for (auto &thread : threads)
	{
		thread.join();
	}

	// Update the buffer index so that the updated state will be rendered
	current_buffer = target_buffer;
}

/*
	Update the state of a single particle based on the forces exerted by all other particles
	in the system. This function is called by each thread in the multi-threaded version.
	Every thread updates a subset of the particles in the system.

	source is the buffer containing the current state of the particles, and target is the buffer
	where the updated state should be written to. The index parameter specifies the index of the
	particle that should be updated.
 */
void ParticleSystem::update_particle(int index, const std::vector<Particle> &source, std::vector<Particle> &target)
{	
	// Get the particle to write to as a reference from the target buffer
	Particle &p = target[index];
	// Copy the particle info from the source buffer
	p = source[index];

	// Update the particle based on the forces exerted by all other particles in the system.
	// Iterates over all particles in the system from the source buffer, except the particle itself.
	for (int j = 0; j < num_particles; ++j)
	{
		if (j == index) continue;
		const Particle &other = source[j];
		// Just a dummy array for function
		std::array<float, 3> p2_vel{0.0f, 0.0f, 0.0f};
		update_particle_pair(p, other, p2_vel);
	}
	update_position(p);
}

// Calculate the force exerted between two particles and updates velocities.
void ParticleSystem::update_particle_pair(Particle &p1, const Particle &p2, std::array<float, 3> &p2_vel)
{
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	float dz = p2.z - p1.z;
	float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
	float force = p1.mass * p2.mass / (dist * dist * dist);

	p1.vx += force * dx;
	p1.vy += force * dy;
	p1.vz += force * dz;

	// Only update the velocity of the second particle if the system is single-threaded.
	// For multi-threaded systems, the second particle will be updated by another thread.
	if (num_threads == 1)
	{
		p2_vel[0] -= force * dx;
		p2_vel[1] -= force * dy;
		p2_vel[2] -= force * dz;
	}
}

// Helper function to update the position of one particle, also constrains the particle 
// to the window dimensions if rendering particle motion.
void ParticleSystem::update_position(Particle &p)
{
	p.x += p.vx;
	p.y += p.vy;
	p.z += p.vz;

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
}