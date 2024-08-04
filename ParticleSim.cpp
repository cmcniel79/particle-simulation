#include "ParticleSim.hpp"

double ParticleSim::benchmark(int num_particles, int num_threads, int num_updates)
{
    ParticleSystem system(num_particles, num_threads, false);
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_updates; ++i)
    {
        system.update();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    return diff.count();
}

void ParticleSim::benchmark_sim()
{
    const int NUM_UPDATES = 100;
    const std::vector<int> particle_counts = {100, 1000, 10000};
    const std::vector<int> thread_counts = {1, 2, 4, 8};

    for (int num_particles : particle_counts)
    {
        std::cout << "\n Number of particles: " << num_particles << "\n";
        // Benchmark multi - threaded version with different thread counts
        for (int num_threads : thread_counts)
        {
            if (num_particles > 1000 and num_threads < 8)
                continue;
            double total_time = benchmark(num_particles, num_threads, NUM_UPDATES);
            std::cout << num_threads << " threads total time: " << total_time 
            << " s, time per update: " << total_time / NUM_UPDATES << " s" << std::endl;
        }
    }
}

void ParticleSim::render_sim(const int num_particles, const int num_threads, const int window_width, const int window_height)
{
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Particle System");
    window.setFramerateLimit(60);

    ParticleSystem system(num_particles, num_threads, true, window_width, window_height);
    ParticleRenderer renderer(system);

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update particle system
        system.update();

        // Clear the window
        window.clear(sf::Color::Black);

        // Draw the particles
        window.draw(renderer);

        // Display the frame
        window.display();

        // Calculate and print FPS
        // TODO: Display only every second 
        float fps = 1.0f / clock.restart().asSeconds();
        renderer.updateFPS(fps);
    }
}