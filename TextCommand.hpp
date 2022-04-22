#pragma once
#include <string>

class TextCommand {
	public:
		enum Mode : short {
			NORMAL = 0,
			INSERTION = 1,
			VISUAL = 2
		};

		TextCommand();
		void writeChar(const wchar_t cc);
		void changeMode(Mode mm);

		const std::wstring getFinalCmdLine();
		const long getCurseur();
		~TextCommand();

		const std::string getCurrentMode();

		const std::wstring rwFindWordArroundCursor();

	private:
		std::wstring m_cmdLine;
		long m_curseur;
		Mode m_mode;

		std::string m_error;
};
