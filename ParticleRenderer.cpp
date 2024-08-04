#include "ParticleRenderer.hpp"

ParticleRenderer::ParticleRenderer(const ParticleSystem &sys) 
	: system(sys), 
	mFont{}, 
	mStatisticsText{} 
	{
	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.0f, 5.0f);
	mStatisticsText.setCharacterSize(30);
}

void ParticleRenderer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	shapes.clear();
	for (auto &p : system.particles[system.current_buffer])
	{
		sf::CircleShape shape(p.mass); // Particle size
		shape.setPosition(static_cast<float>(p.x), static_cast<float>(p.y));
		// Random Color
		shape.setFillColor(p.color);
		shapes.push_back(shape);
	}
	for (auto &shape : shapes)
	{
		target.draw(shape, states);
	}
	target.draw(mStatisticsText);
}

void ParticleRenderer::updateFPS(float fps)
{
		mStatisticsText.setString("Frames / Second = " + std::to_string(fps) + "\n");
}