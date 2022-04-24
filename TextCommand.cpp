#include "TextCommand.hpp"
#include <functional>
#include <iostream>
#include <locale>
#include <string>

TextCommand::TextCommand() {
	m_cmdLine = L"";
	m_curseur = 0;

	m_mode = NORMAL;
}

void TextCommand::writeChar(const wchar_t cc) {
	if (m_mode == INSERTION) {
		if (cc == 27) {
			m_mode = NORMAL;
			if (m_curseur) {
				m_curseur--;
			}
			return;
		} else if (cc == 127) {
			if (m_curseur+1 < m_cmdLine.size()) {
				m_cmdLine.erase(m_curseur+1, 1);
			}
		} else if (cc == 8) {
			if (m_curseur != 0) {
				m_curseur--;
				m_cmdLine.erase(m_curseur, 1);
			}
		} else {
			std::wstring ui;
			ui += cc;
			m_cmdLine.insert(m_curseur, ui.c_str());
			m_curseur++;
		}
	} else if (m_mode == NORMAL) {
		if (cc == L'i') {
			m_mode = INSERTION;
		} else if (cc == L'h') {
			if (m_curseur != 0)
				m_curseur--;
		} else if (cc == L'l' || cc == 32) {
			if (m_curseur+1 < m_cmdLine.size())
				m_curseur++;
		} else if (cc == L'a') {
			if (0 != m_cmdLine.size()) {
				m_curseur++;
			}
			m_mode = INSERTION;
		} else if (cc == L'I') {
			m_curseur = 0;
			m_mode = INSERTION;
		} else if (cc == L'A') {
			m_curseur = m_cmdLine.size();
			m_mode = INSERTION;
		} else if (cc == L'~') {
			if (m_curseur < m_cmdLine.size() && m_curseur >= 0) {
				wchar_t &c_char = m_cmdLine[m_curseur];

				if (64 < c_char && c_char < 91) {
					c_char += 32;
				} else if (96 < c_char && c_char < 123) {
					c_char -= 32;
				}

				if (m_curseur+1 < m_cmdLine.size())
					m_curseur++;
			}
		} else if (cc == L'$') {
			if (m_cmdLine.size() != 0)
				m_curseur = m_cmdLine.size() - 1;
		} else if (cc == L'^' || cc == L'0') {
			m_curseur = 0;
		} else if (cc == L'#') {
			while (m_cmdLine[m_curseur] == L' ') {
				m_curseur++;
				if (m_curseur == m_cmdLine.size()) {
					m_curseur = m_cmdLine.size() - 1;
					return;
				}
			}

			const std::wstring word {rwFindWordArroundCursor()};

			std::string::size_type posWord { std::string::npos };
			
			if (m_curseur - word.size() >= 0)
				posWord = m_cmdLine.rfind(word, m_curseur - word.size());

			if (posWord == std::string::npos) {
				posWord = m_cmdLine.rfind(word, m_cmdLine.size() - 1);
			}

			m_curseur = posWord;
		} else if (cc == L'%') {
			std::reference_wrapper<wchar_t> c_char = std::ref(m_cmdLine[m_curseur]);
			auto sstart {m_curseur};

			while (c_char.get() != 40 && c_char.get() != 41 &&
				c_char.get() != 91 && c_char.get() != 93 &&
				c_char.get() != 123 && c_char.get() != 125) {

				m_curseur++;

				if (sstart == m_curseur) {
					return;
				} else if (m_curseur == m_cmdLine.size()) {
					m_curseur = 0;
				}

				c_char = std::ref(m_cmdLine[m_curseur]);
			}

			if (sstart != m_curseur) {
				return;
			}

			wchar_t inverseFind {0};
			int occ {1};
			int adv {0};

			switch (c_char) {
				case 40:
					adv = 1;
					inverseFind = 41;
					break;
				case 41:
					adv = -1;
					inverseFind = 40;
					break;
				case 91:
					adv = 1;
					inverseFind = 93;
					break;
				case 93:
					adv = -1;
					inverseFind = 91;
					break;
				case 123:
					adv = 1;
					inverseFind = 125;
					break;
				case 125:
					adv = -1;
					inverseFind = 123;
					break;
				default:
					std::cout << "not found\n";
					return;
			}
		
			for (m_curseur += adv;
					(m_curseur > 0 || adv == 1) && (m_curseur < m_cmdLine.size()-1 || adv == -1);
					m_curseur += adv) {

				if (m_cmdLine[m_curseur] == c_char.get()) {
					occ++;
				} else if (m_cmdLine[m_curseur] == inverseFind) {
					occ--;

					if (occ == 0) {
						return;
					}
				}
			}

		} else if (cc == L'w') {
			bool space {false};
			for (;m_curseur < m_cmdLine.size();m_curseur++) {
				if (m_cmdLine[m_curseur] == L' ') {
					space = true;
				} else if (space) {
					return;
				}
			}
		} else if (cc == L'b') {
			int space {0};
			for (;m_curseur > 0;m_curseur--) {
				if (m_cmdLine[m_curseur] == L' ') {
					if (space == 0) {
						space = 1;
					} else {
						m_curseur++;
						return;
					}
				} else if (space == 1) {
					if (m_cmdLine[m_curseur] != L' ') {
						space = 2;
					}
				} 
			}
		}
	}
		
	//std::cout << m_curseur << " " << m_cmdLine.size() << "\n";
}

void TextCommand::changeMode(Mode mm) {
	m_mode = mm;
}

void TextCommand::clear() {
	m_cmdLine.clear();
	m_mode = NORMAL;
	m_curseur = 0;
	m_error.clear();
}

const std::wstring TextCommand::getFinalCmdLine() {
	return m_cmdLine;
}

const long TextCommand::getCurseur() {
	return m_curseur;
}

const std::string TextCommand::getCurrentMode() {
	if (m_mode == INSERTION) {
		return "INSERTION" + m_error;
	} else if (m_mode == NORMAL) {
		return "NORMAL" + m_error;
	}

	return "VISUAL" + m_error;
}

const std::wstring TextCommand::rwFindWordArroundCursor() {
	std::string::size_type start;
	std::string::size_type end;

	start = m_cmdLine.rfind(' ', m_curseur);
	if (start == std::string::npos) {
		start = 0;
	} else {
		start++;
	}

	end = m_cmdLine.find(' ', m_curseur);
	if (end == std::string::npos) {
		end = m_cmdLine.size();
	}

	return m_cmdLine.substr(start, end - start);
}

TextCommand::~TextCommand() {}
