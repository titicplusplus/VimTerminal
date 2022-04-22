#include "Interact.hpp"
#include <iostream>

#include <unistd.h>
#include <cstdlib>
#include <cerrno>

#include <sys/wait.h>

InteractLinux::InteractLinux() { 
	int pipe_stdin[2], pipe_stdout[2];

	if (pipe(pipe_stdin)) {
		std::cerr << "Can't create the stdin pipe\n";
	       	return;	
	}
	if (pipe(pipe_stdout)) {
		std::cerr << "Can't create the stdout pipe\n";
		return;
	}

	m_pipe = fork();

	if (m_pipe < 0) {
		std::cerr << "Can't create the fork\n";
		return;
	} else if (m_pipe == 0) {
		close(pipe_stdin[1]);
		dup2(pipe_stdin[0], 0);
		close(pipe_stdout[0]);
		dup2(pipe_stdout[1], 1);

		execl("/bin/zsh", "zsh", NULL);
		perror("execl"); exit(99);
	}


	m_pipe_stdin  = pipe_stdin[1];
	m_pipe_stdout = pipe_stdout[0];
	close(pipe_stdin[0]);
	close(pipe_stdout[1]);
}

void InteractLinux::toWrite(const std::string cmd) {
	write(m_pipe_stdin, cmd.c_str(), cmd.size());
}

void InteractLinux::waitPipe() {
	int status;
	waitpid(m_pipe, &status, WUNTRACED | WCONTINUED);
	std::cout << "The status is :" << status << "\n";
}

std::string InteractLinux::getResult() {
	char buf[1000];

	int i = read(m_pipe_stdout, buf, 1000);

	std::string result { buf};

	return result;
}

InteractLinux::~InteractLinux() {
}
