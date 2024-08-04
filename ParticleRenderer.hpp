#include <SFML/Graphics.hpp>
#include "ParticleSystem.hpp"

class ParticleRenderer : public sf::Drawable
{
public:
    ParticleRenderer(const ParticleSystem &sys);
	void updateFPS(float fps);

private:
    const ParticleSystem &system;
    mutable std::vector<sf::CircleShape> shapes;
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    sf::RenderWindow mWindow;
	sf::Font mFont;
    sf::Text mStatisticsText;
};
