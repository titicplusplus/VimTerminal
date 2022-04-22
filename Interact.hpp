#pragma once
#include <cstdio>
#include <memory>

#include <string>

class InteractLinux {

	pid_t m_pipe;
	int m_pipe_stdin, m_pipe_stdout;

	public:
		InteractLinux();

		void toWrite(const std::string cmd);
		std::string getResult();

		void waitPipe();

		~InteractLinux();
};
