#include "Interact.hpp"
#include <future>
#include <iostream>

#include <unistd.h>
#include <cstdlib>
#include <cerrno>

#include <sys/wait.h>

void freeParser(char ** &parserCmdLine, int size) {
	std::cout << "CC" << "\n";
	for (int i {0}; i < size; i++) {
		std::cout << parserCmdLine[i] << "\n";
		free(parserCmdLine[i]);
	}

	free(parserCmdLine);
}
		
std::string executeCMD(std::vector<std::string> &str_parserCmdLine) {//;std::wstring_view cmdLine) {
	/**for (const auto &ui : parserCmdLine) {
		std::cout << ui << " ";
	}**/

	char **parserCmdLine = (char**)malloc((str_parserCmdLine.size()+1) * sizeof(char*));

	{

		std::size_t i;
		for (i = 0; i < str_parserCmdLine.size(); i++) {
			//parserCmdLine[i] = str_parserCmdLine[i].c_str();

			parserCmdLine[i] = (char*)malloc((str_parserCmdLine[i].size() + 1)*sizeof(char));

			std::size_t iStr {0};
			for (;iStr < str_parserCmdLine[i].size(); iStr++) {
				parserCmdLine[i][iStr] = str_parserCmdLine[i][iStr];
			}

			parserCmdLine[i][iStr] = '\0';
		}

		parserCmdLine[i] = NULL;
	}

	pid_t m_pipe;
	int m_pipe_stdin, m_pipe_stdout;

	int pipe_stdin[2], pipe_stdout[2];

	if (pipe(pipe_stdin)) {
		std::cerr << "Can't create the stdin pipe\n";

		freeParser(parserCmdLine, str_parserCmdLine.size());

	       	return ":!";
	}
	if (pipe(pipe_stdout)) {
		std::cerr << "Can't create the stdout pipe\n";
		
		freeParser(parserCmdLine, str_parserCmdLine.size());

		return ":!";
	}

	bool *cmdFound = (bool*)malloc(sizeof(bool));
	*cmdFound = true;

	m_pipe = fork();

	if (m_pipe < 0) {
		std::cerr << "Can't create the fork\n";

		free(cmdFound);

		freeParser(parserCmdLine, str_parserCmdLine.size());

		/**for (std::size_t i {0}; i < str_parserCmdLine.size(); i++) {
			parserCmdLine[i] = str_parserCmdLine[i].c_str();
		}**/


		return ":!";
	} else if (m_pipe == 0) {
		close(pipe_stdin[1]);
		dup2(pipe_stdin[0], 0);
		close(pipe_stdout[0]);
		dup2(pipe_stdout[1], 1);

		if (parserCmdLine[0][0] == '.') {
			if (execvp(parserCmdLine[0], parserCmdLine) == -1) {
				*cmdFound = false;
			}
		} else {
			if (execvp((std::string("/bin/") + parserCmdLine[0]).c_str(), parserCmdLine) == -1) {
				*cmdFound = false;
			}
		}
	
		exit(EXIT_FAILURE);
	} else {
		m_pipe_stdin  = pipe_stdin[1];
		m_pipe_stdout = pipe_stdout[0];
		close(pipe_stdin[0]);
		close(pipe_stdout[1]);

		int status;

		pid_t wpid;
		do {
			wpid = waitpid(m_pipe, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		std::cout << "The result is:" << (*cmdFound) << "\n";

		freeParser(parserCmdLine, str_parserCmdLine.size());
		/**for (auto &ui : parserCmdLine) {
			free(ui);
		}**/

		bool value = *cmdFound;
		free(cmdFound);

		if (value) {
			char buf[4096];
			int i = read(m_pipe_stdout, buf, 4096);
			std::cout << "I => " << i << "\n";
			
			return std::string {buf, buf + i};
		} else {
			std::cout << "Erreur" << "\n";
		}
			
		return ":!";
	}
}

std::string InteractLinux::newCommand(std::wstring_view cmdLine) { 
	/**cmdExecute = std::async(&executeCMD, cmdLine);

	cmdExecute.wait();
	std::string rfinal {cmdExecute.get()};
	
	return rfinal;**/
}

void InteractLinux::toWrite(const std::string cmd) {
	write(m_pipe_stdin, cmd.c_str(), cmd.size());
}

int InteractLinux::waitPipe() {
	int status;

	pid_t wpid;
	do {
		wpid = waitpid(m_pipe, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	std::cout << "The status is :" << status << "\n";

	return status;
}

std::string InteractLinux::getResult() {
	char buf[1000];

	int i = read(m_pipe_stdout, buf, 1000);

	std::string result { buf};

	return result;
}
