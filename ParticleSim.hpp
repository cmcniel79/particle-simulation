#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <functional>
#include "ParticleRenderer.hpp"

class ParticleSim
{
public:
    void benchmark_sim();
    void render_sim(const int num_particles, const int num_threads, const bool use_bh, const int window_width, const int window_height);

private:
    double benchmark(int num_particles, int num_threads, int num_updates);
};