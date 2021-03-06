#include "TextBox.hpp"
#include <string.h>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <experimental/bits/fs_ops.h>
#include <experimental/bits/fs_path.h>
#include <iostream>

#include <string>
#include <vector>


TextBox::TextBox(sf::Vector2f positon, sf::Vector2u size, const Ressources &res) : m_res{res} {
	resize(positon, size);

	textMode.setFont(m_res.getCurrentFont());
	textMode.setCharacterSize(m_res.getFontHeigthSize());
	textMode.setString(m_vimText.getCurrentMode());

	m_shiftStart = 0;
	m_shiftEnd = 1;
	//defaultEnter = L"⬖❯ ";

	newText();
			
	currentPath = fs::current_path();

	defaultEnter = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(currentPath.filename()) + L" >> ";

	m_textSFML[0].setString(defaultEnter);
	findShift();

	canWrite = true;

}

void TextBox::newText() {
	std::size_t i { m_textSFML.size() };

	m_textSFML.push_back(sf::Text());
	m_textSFML[i].setFont(m_res.getCurrentFont());
	m_textSFML[i].setCharacterSize(m_res.getFontHeigthSize());
	m_textSFML[i].setPosition(
			sf::Vector2f(m_positon.x + m_res.getMargin(),
				m_positon.y + m_res.getMargin() + i*(m_res.getFontHeigthSize() + 5)));

}

void TextBox::findShift() {
	const unsigned int textSize { (m_res.getFontHeigthSize() + 5)*m_textSFML.size() };

	if (textSize > m_size.y - m_res.getFontHeigthSize() - 2*m_res.getMargin()) {
		m_shiftEnd = m_textSFML.size();
		m_shiftStart = std::ceil(m_shiftEnd - (m_size.y - m_res.getFontHeigthSize() - 2*m_res.getMargin())/
			(m_res.getFontHeigthSize() + 5));

	} else {
		m_shiftStart = 0;
		m_shiftEnd = m_textSFML.size();
	}

	for (std::size_t s {m_shiftStart}; s < m_shiftEnd; s++) {
		m_textSFML[s].setPosition(sf::Vector2f(m_positon.x + m_res.getMargin(),
			m_positon.y + m_res.getMargin() + (s - m_shiftStart)*(m_res.getFontHeigthSize()+5)));
	}

	textMode.setPosition(sf::Vector2f(m_res.getMargin(), m_positon.y + m_size.y - m_res.getFontHeigthSize() - m_res.getMargin()));
}

void TextBox::move() {
	if (!canWrite) {
		std::string rcmdLine = m_cmdRun.getResult();

		if (!rcmdLine.empty()) {
			std::size_t prev_pos = 0;
			std::size_t pos = 0;

			while((pos = rcmdLine.find("\n", pos)) != std::string::npos) {
				const std::string substring {rcmdLine.substr(prev_pos, pos-prev_pos)};

				if (newLine == 1) {
					newText();
					m_textSFML[m_textSFML.size() - 1].setString(
							std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(substring));

				} else if (newLine == 0) {
					m_textSFML[m_textSFML.size() - 1].setString(
							m_textSFML[m_textSFML.size() - 1].getString() +
							std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(substring));

					newLine = 1;
				} else {
					std::cout << "har\n";
					newLine = 1;
				}
					
				prev_pos = ++pos;
			}

			{
				const std::string substring {rcmdLine.substr(prev_pos, pos-prev_pos)};
				if (!substring.empty()) {
					newText();
					m_textSFML[m_textSFML.size() - 1].setString(
						std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(substring));
				}

				newLine = false;

				findShift();
			}
		} else if (m_cmdRun.isFinish()) {
			canWrite = true;

			newText();
			findShift();
			m_vimText.clear();
			m_textSFML[m_textSFML.size() - 1].setString(defaultEnter);
			textMode.setString(m_vimText.getCurrentMode());

			newLine = 2;
		}
	}
}

int TextBox::sendText(const wchar_t cc) {
	if (canWrite) {
		if (cc == 13) {
			std::string cmdLine {std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(m_vimText.getFinalCmdLine())};
			std::string::size_type prev_pos = 0, pos = 0;

			std::vector<std::string> parserCmdLine;

			while((pos = cmdLine.find(" ", pos)) != std::string::npos) {
				const std::string substring {cmdLine.substr(prev_pos, pos-prev_pos)};

				parserCmdLine.push_back(substring);

				prev_pos = ++pos;
			}

			{
				const std::string substring {cmdLine.substr(prev_pos, pos-prev_pos)};

				
				parserCmdLine.push_back(substring);
			}

			if (parserCmdLine.size() == 0) {
				return 0;
			} 

			if (parserCmdLine[0].size() == 0) {
			} else if (parserCmdLine[0] == "exit") {
				return 1;
			} else if (parserCmdLine[0] == "cd") {
				if (parserCmdLine.size() == 1) {
					currentPath = fs::path("/home/famillevincent/");
				} else if (parserCmdLine.size() == 2 && fs::exists(parserCmdLine[1])) {
					if (parserCmdLine[1][0] == '/') {
						currentPath = parserCmdLine[1];
					} else {
						currentPath.append(parserCmdLine[1]);
					}
				} else {
					newText();
					m_textSFML[m_textSFML.size() - 1].setString("path not found");

					newText();
					findShift();
					m_vimText.clear();
					m_textSFML[m_textSFML.size() - 1].setString(defaultEnter);
					textMode.setString(m_vimText.getCurrentMode());
					
					return 0;
				}

				currentPath = fs::canonical(currentPath);
					
				defaultEnter = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(
						currentPath.filename()) + L" >> ";
				fs::current_path(currentPath);

				newText();
				findShift();
				m_vimText.clear();
				m_textSFML[m_textSFML.size() - 1].setString(defaultEnter);
				textMode.setString(m_vimText.getCurrentMode());

			} else {
				if (fs::exists("/bin/" + parserCmdLine[0]) || (parserCmdLine[0][0] == '.' && fs::exists(parserCmdLine[0]))) {
					if (m_cmdRun.newCommand(parserCmdLine)) {
						canWrite = false;
					}
				} else {
					newText();
					m_textSFML[m_textSFML.size() - 1].setString("command not found");

					newText();
					findShift();
					m_vimText.clear();
					m_textSFML[m_textSFML.size() - 1].setString(defaultEnter);
					textMode.setString(m_vimText.getCurrentMode());

				}

				
			}

		} else {
			m_vimText.writeChar(cc);

			textMode.setString(m_vimText.getCurrentMode());
			m_textSFML[m_textSFML.size() - 1].setString(defaultEnter + m_vimText.getFinalCmdLine());
		}
	} else {
		tmpCC.push_back(cc);
	}
				
	return 0;
}

void TextBox::resize(sf::Vector2f positon, sf::Vector2u size) {
	m_positon = positon;
	m_size    = size;
	findShift();
}

void TextBox::draw(sf::RenderWindow &window) const {
	for (std::size_t s {m_shiftStart}; s < m_shiftEnd; s++) {
		window.draw(m_textSFML[s]);
	}

	window.draw(textMode);
}

sf::Vector2f TextBox::actualCursorPosition() {
	if (canWrite) {
		return
			sf::Vector2f((m_vimText.getCurseur() + defaultEnter.size()) * m_res.getFontWeigthSize(), 
				m_textSFML[m_textSFML.size() - 1].getPosition().y);
	} else {
		return sf::Vector2f(-1000, -1000);
	}
}

TextBox::~TextBox() {
}
