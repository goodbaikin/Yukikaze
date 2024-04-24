#include "subprocess.hpp"
#include "spdlog/spdlog.h"
#include <string>
#include <signal.h>
#include <sys/wait.h>
#include <filesystem>

yukikaze::SubProcess::SubProcess(std::filesystem::path logpath, const std::vector<std::string> & args) {
	pid_ = -1;
	status_  = 0;
	char** argv = new char*[args.size()+1];
	const char* cmd = args[0].c_str();
	int fd;

	pid_t pid = fork();
	switch (pid) {
	case -1:
		perror("fork()");
		throw(std::runtime_error("fork failed"));
		break;
	case 0:
		fd = open(logpath.c_str(), O_CREAT|FD_CLOEXEC, 0600);
		if (fd < 0) {
			perror("open()");
			throw(std::runtime_error("open log file failed"));
		}

		if (dup2(fd, 2) < 0) {
			perror("dup2()");
			throw(std::runtime_error("dup2 failed"));
		}
		for (int i = 0; i < args.size(); i++) {
			argv[i] = strdup(args[i].c_str());
		}
		argv[args.size()] = NULL;
		if (execvp(cmd, argv) < 0) {
			perror("failed to launch Amatsukaze");
			throw(std::runtime_error("failed to launch Amatsukaze"));
		}
		break;
	default:
		pid_ = pid;
		break;
	}
}

yukikaze::SubProcess::~SubProcess() {
	join();
	pid_ = -1;
}

bool yukikaze::SubProcess::isRunning() {
	if (pid_ == -1) {
		return false;
	}
	if (kill(pid_, 0) == -1) {
		return false;
	}
	return true;
}

void yukikaze::SubProcess::join() {
	waitpid(pid_, &status_, 0);
}

int yukikaze::SubProcess::cancel() {
	kill(pid_, SIGTERM);
	return exitCode();
}

int yukikaze::SubProcess::exitCode() {
	if (isRunning()) {
		join();
	}
	return WEXITSTATUS(status_);
}
