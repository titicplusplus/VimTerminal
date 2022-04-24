#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>

class CursorRect {
	sf::RectangleShape m_rect;

	std::chrono::steady_clock::time_point m_time;
	short m_draw;

	public:
		CursorRect(float size, float heigth);
		void move(const sf::Vector2f &pos, int currentPosition, float size);
		void currentWrite();
		void draw(sf::RenderWindow &window);
};
