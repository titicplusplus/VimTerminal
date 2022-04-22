#include "CursorRect.hpp"
#include <SFML/System/Vector2.hpp>
#include <chrono>


CursorRect::CursorRect(float size, int heigth) {
	m_time = std::chrono::steady_clock::now();
	m_rect.setSize(sf::Vector2f(size, heigth));
}
	
void CursorRect::move(const sf::Vector2f &pos, int currentPosition, float size) {
	m_rect.setPosition(sf::Vector2f(pos.x + static_cast<float>(currentPosition)*size, pos.y));
}

void CursorRect::currentWrite() {
	m_time = std::chrono::steady_clock::now();
	m_draw = 1;
}

void CursorRect::draw(sf::RenderWindow &window) {
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_time).count() > 500) {
		m_draw = (m_draw+1)%2;
		m_time = std::chrono::steady_clock::now();
	}

	if (m_draw) {
		window.draw(m_rect);
	}
}
