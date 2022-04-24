#pragma once
#include <SFML/Graphics/Font.hpp>

class Ressources {
	const float textFontSize;
	const float textSize;

	sf::Font m_font;

	public:
		Ressources();
		float getFontHeigthSize() const;
		float getFontWeigthSize() const;

		const sf::Font& getCurrentFont() const;
		const int getMargin() const;
};

