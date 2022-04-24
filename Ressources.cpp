#include "Ressources.hpp"
#include <iostream>

Ressources::Ressources() : textFontSize {34}, textSize {21.03125} {
	if (!m_font.loadFromFile("fonts/Sono-Regular.ttf")) {
		std::cerr << "Can't open fond" << "\n";
	}
}

float Ressources::getFontHeigthSize() const {
	return textFontSize;
}

float Ressources::getFontWeigthSize() const {
	return textSize;
}

const sf::Font& Ressources::getCurrentFont() const {
	return m_font;
}

const int Ressources::getMargin() const {
	return 10;
}
