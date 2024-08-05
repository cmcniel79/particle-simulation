#include <vector>
#include "Particle.hpp"

class QuadTreeNode {
public:
    float center_x, center_y;
    float size;
    float total_mass;
    float center_of_mass_x, center_of_mass_y;
    Particle particle;
    std::vector<QuadTreeNode> children[4];

    QuadTreeNode(float x, float y, float s);

private:
    void divide_quadrants();
};