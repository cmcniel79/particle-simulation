#include "ParticleSim.hpp"

int main() {
	ParticleSim sim{};
	// sim.benchmark_sim();
	// Multi-Threaded Sim w/ 8 threads
	sim.render_sim(1000, 8, false, 1500, 1500);
	return 0; 
}
