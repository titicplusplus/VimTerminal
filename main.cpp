#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Interact.hpp"
#include "TextCommand.hpp"
#include "CursorRect.hpp"
#include <unistd.h>
#include <csignal>

#include <thread>

bool contineAlive {true};

void signalEnd(int signal) {
	contineAlive = false;
	std::cout << "Stop by user" << "\n";
}

int main() {
	//std::signal(SIGINT, signalEnd);

	/**InteractLinux cmd;

	cmd.toWrite("w\n");
	cmd.toWrite("make\n");
	cmd.toWrite("pwd\n");

	while (contineAlive) {
		std::string command { cmd.getResult() };

		std::cout << command;

		if (command.size() > 2) {
			std::cout << int(command[command.size() - 2]) << " "
			<< int(command[command.size() - 1]) << "\n";
		}
	}

	return 0;**/

	sf::VideoMode desktop = sf::VideoMode().getDesktopMode();
	sf::RenderWindow window(sf::VideoMode(800, 600), "CppMinTerminal", sf::Style::Default);
	window.setFramerateLimit(60);

	sf::View viewTerminal { sf::FloatRect(0, 0, window.getSize().x, window.getSize().y) };

	const int textFontSize = 34;
	const float textSize = 21.03125;

	sf::Font font;
	if (!font.loadFromFile("fonts/Sono-Regular.ttf")) {
		std::cerr << "Can't open fond" << "\n";
		return -1;
	}

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(textFontSize);
	text.setPosition(sf::Vector2f(10, 10));

	sf::Text textMode;
	textMode.setFont(font);
	textMode.setCharacterSize(textFontSize);
	textMode.setPosition(sf::Vector2f(10, window.getSize().y - 10 - textFontSize));

	TextCommand vimText;
	textMode.setString(vimText.getCurrentMode());

	CursorRect rectCursor {textSize, 34};
	rectCursor.move(text.getPosition(), vimText.getCurseur(), textSize);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::TextEntered) {
				std::cout << "ASCII character typed: " << static_cast<wchar_t>(event.text.unicode) << "\n";
				if (event.text.unicode == 13) {
				} else {
					vimText.writeChar(event.text.unicode);
					rectCursor.currentWrite();

					text.setString(vimText.getFinalCmdLine());
					textMode.setString(vimText.getCurrentMode());
					rectCursor.move(text.getPosition(), vimText.getCurseur(), textSize);
				}
			} if (event.type == sf::Event::Resized) {
				viewTerminal.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
				textMode.setPosition(sf::Vector2f(10, window.getSize().y - 10 - textFontSize));
		       	}
		}

		window.clear(sf::Color::Black);

		window.setView(viewTerminal );

		rectCursor.draw(window);
		window.draw(text);
		window.draw(textMode);

        	window.display();
    	}

	return 0;
}
