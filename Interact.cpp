#include "Interact.hpp"
#include <future>
#include <iostream>

#include <memory>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>

#include <sys/wait.h>

void freeParser(char ** &parserCmdLine, int size) {
	for (int i {0}; i < size; i++) {
		std::cout << parserCmdLine[i] << "\n";
		free(parserCmdLine[i]);
	}

	free(parserCmdLine);
}
		
std::mutex InteractLinux::all_m;
std::thread *InteractLinux::go_waidPID;

InteractLinux::InteractLinux() {}

InteractLinux::~InteractLinux() {}

bool InteractLinux::newCommand(std::vector<std::string> &str_parserCmdLine) {
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

	int pipe_stdin[2], pipe_stdout[2], pipe_stderr[2];

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

	if (pipe(pipe_stderr)) {
		std::cerr << "Can't create the stderr pipe\n";

		freeParser(parserCmdLine, str_parserCmdLine.size());

		return ":!";
	}


	m_pipe = fork();

	if (m_pipe < 0) {
		std::cerr << "Can't create the fork\n";

		freeParser(parserCmdLine, str_parserCmdLine.size());

		return false;
	} else if (m_pipe == 0) {
		close(pipe_stdin[1]);
		dup2(pipe_stdin[0], 0);

		close(pipe_stdout[0]);
		dup2(pipe_stdout[1], 1);

		close(pipe_stderr[0]);
		dup2(pipe_stderr[1], STDERR_FILENO);

		printf("\n");

		if (parserCmdLine[0][0] == '.') {
			if (execvp(parserCmdLine[0], parserCmdLine) == -1) {
			}
		} else {
			if (execvp((std::string("/bin/") + parserCmdLine[0]).c_str(), parserCmdLine) == -1) {
			}
		}
	
		exit(EXIT_FAILURE);
	} else {
		m_pipe_stdin  = pipe_stdin[1];
		m_pipe_stdout = pipe_stdout[0];
		m_pipe_stderr = pipe_stderr[0];

		close(pipe_stdin[0]);
		close(pipe_stdout[1]);
		close(pipe_stderr[1]);


		finishProcess = false;

		go_waidPID = new std::thread(waitPipe, m_pipe, std::ref(finishProcess));

		freeParser(parserCmdLine, str_parserCmdLine.size());

		return true;
	}
}

bool InteractLinux::isFinish() {
	const std::lock_guard<std::mutex> ll {InteractLinux::all_m};
	return finishProcess;
}

void InteractLinux::toWrite(const std::string cmd) {
	write(m_pipe_stdin, cmd.c_str(), cmd.size());
}

std::string InteractLinux::getResult() {
	char buf[1000];

	/**int i = read(m_pipe_stderr, buf, 1000);
	
	std::string result {buf};

	return result;**/

	int i = read(m_pipe_stderr, buf, 1000);

	if (i == -1) {
		return "";
	}

	std::string result {buf, buf+i};
	
	i = read(m_pipe_stdout, buf, 1000);

	if (i == -1) {
		return result;
	}
	
	result.append({buf, buf+i});
	
	return result;

}

void waitPipe(int pipe, bool &end) {
	pid_t wpid;
	int status;

	do {
		wpid = waitpid(pipe, &status, WUNTRACED | WCONTINUED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	const std::lock_guard<std::mutex> ll {InteractLinux::all_m};
	end = true;
}
