#include "QuadTreeNode.hpp"
#include "Particle.hpp"

QuadTreeNode::QuadTreeNode(float x, float y, float s) 
    : center_x{x},
      center_y{y},
      size{s},
      total_mass{0},
      center_of_mass_x{0},
      center_of_mass_y{0},
      children{}{}

 void divide_quadrants()
 {
 }