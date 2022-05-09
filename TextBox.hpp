#pragma once

#define MARGIN 10

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>

#include "TextCommand.hpp"
#include "Ressources.hpp"
#include "Interact.hpp"

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif
	

class TextBox {
	std::vector<sf::Text> m_textSFML;
	TextCommand m_vimText;
	sf::Vector2f m_positon;
	sf::Vector2u m_size;

	sf::Text textMode;

	const Ressources &m_res;
	std::wstring defaultEnter;

	std::size_t m_shiftStart;
	std::size_t m_shiftEnd;

	bool canWrite {true};
	int newLine {2};

	std::vector<wchar_t> tmpCC;

	void newText();
	void findShift();

	fs::path currentPath;

	InteractLinux m_cmdRun;

	public:
		TextBox(sf::Vector2f positon, sf::Vector2u size, const Ressources &res);
		~TextBox();

		int sendText(const wchar_t cc);
		void resize(sf::Vector2f positon, sf::Vector2u size);
		void draw(sf::RenderWindow &window) const;

		void move();

		sf::Vector2f actualCursorPosition();
};
