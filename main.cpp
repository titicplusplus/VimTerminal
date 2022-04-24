#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

//#include "Interact.hpp"
//#include "TextCommand.hpp"
#include "CursorRect.hpp"
#include <unistd.h>
#include <csignal>

#include "TextBox.hpp"

#include <thread>

#include "Ressources.hpp"

bool contineAlive {true};

void signalEnd(int signal) {
	contineAlive = false;
	std::cout << "Stop by user" << "\n";
}

int main() {
	//std::signal(SIGINT, signalEnd);

	sf::VideoMode desktop = sf::VideoMode().getDesktopMode();
	sf::RenderWindow window(desktop, "CppMinTerminal", sf::Style::Default);
	window.setFramerateLimit(60);

	sf::View viewTerminal { sf::FloatRect(0, 0, window.getSize().x, window.getSize().y) };

	Ressources res;

	CursorRect rectCursor {res.getFontWeigthSize(), res.getFontHeigthSize()};
	//rectCursor.move(text.getPosition(), vimText.getCurseur(), res.getFontWeigthSize());
	
	int currentFocus = 0;

	std::vector<TextBox> m_textBox;

	m_textBox.push_back( TextBox{sf::Vector2f(0, 0), window.getSize(), res} );

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::TextEntered) {
				std::cout << "ASCII character typed: " << event.text.unicode << "\n";
				
				int i = m_textBox[currentFocus].sendText(event.text.unicode);

				if (i == 1) {
					window.close();
				}
			}
		       
			if (event.type == sf::Event::Resized) {
				viewTerminal.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));

				for (auto &ttBox : m_textBox) {
					ttBox.resize(sf::Vector2f(0, 0), window.getSize());
				}

		       	}
		}

		window.clear(sf::Color::Black);

		window.setView(viewTerminal );

		//rectCursor.draw(window);
	
		for (const auto &ttBox : m_textBox) {
			ttBox.draw(window);
		}

        	window.display();
    	}

	return 0;
}
