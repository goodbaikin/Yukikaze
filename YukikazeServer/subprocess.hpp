#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <filesystem>
#include <vector>

namespace yukikaze {

class SubProcess {
public:
	SubProcess(std::filesystem::path, const std::vector<std::string> &);
	~SubProcess();
	bool isRunning();
	void join();
	int cancel();
	int exitCode();
private:
	int pid_;
	int status_;
};

}
#endif