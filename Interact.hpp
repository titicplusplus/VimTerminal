#pragma once
#include <vector>
#include <cstdio>
#include <memory>
#include <thread>

#include <string>

std::string executeCMD(std::vector<std::string> &str_parserCmdLine); //;std::wstring_view cmdLine);
void executeCommand(char**, int size);

class InteractLinux {

	std::thread currentCmdProcess;

	pid_t m_pipe;
	int m_pipe_stdin, m_pipe_stdout;

	public:
		std::string newCommand(std::wstring_view cmdLine);

		void toWrite(const std::string cmd);
		std::string getResult();
		bool isAlive();

		int waitPipe();
};
