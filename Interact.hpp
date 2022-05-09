#pragma once
#include <vector>
#include <cstdio>
#include <memory>

#include <thread>
#include <mutex>

#include <string>

std::string executeCMD(std::vector<std::string> &str_parserCmdLine); //;std::wstring_view cmdLine);
void executeCommand(char**, int size);



class InteractLinux {
	pid_t m_pipe;
	int m_pipe_stdin, m_pipe_stdout, m_pipe_stderr;

	bool result;
	bool finishProcess;

	static std::thread *go_waidPID;

	public:
		static std::mutex all_m;

		InteractLinux();
		~InteractLinux();
		bool newCommand(std::vector<std::string> &str_parserCmdLine);

		void toWrite(const std::string cmd);
		std::string getResult();
		bool isFinish();
};

void waitPipe(int pipe, bool &end);
