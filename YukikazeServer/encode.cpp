#include <filesystem>
#include <grpc/status.h>

#include "spdlog/spdlog.h"
#include "api/yukikaze.grpc.pb.h"
#include "yukikaze_server.hpp"
#include <sys/wait.h>

using namespace yukikaze;

std::string YukikazeServiceImpl::decoderToStr(Decoder decoder) {
	switch (decoder) {
	case QSV:
		return std::string("QSV");
	case CUVID:
		return std::string("CUVID");
	case DEFAULT:
	default:
		return std::string("default");
	}
}

std::string YukikazeServiceImpl::encoderTypeToStr(EncoderType encoderType) {
	switch (encoderType) {
	case X265:
		return std::string("x265");
	case QSVENC:
		return std::string("QSVEnc");
	case NVENC:
		return std::string("NVEnc");
	case VCEENC:
		return std::string("VCEEnc");
	case SVT_AV1:
		return std::string("SVT-AV1");
	case X264:
	default:
		return std::string("x264");
	}
}

void YukikazeServiceImpl::parseRequest(const EncodeRequest* request, std::vector<std::string> & args) {
	args.clear();
	args.push_back("amatsukaze");

	// input path
	std::filesystem::path inputpath = (recordedpath_ / request->input_name()).lexically_normal();
	args.push_back("-i");
	args.push_back(inputpath.string());

	// output path
	std::filesystem::path outputpath = (recordedpath_ / request->output_name()).lexically_normal();
	args.push_back("-o");
	args.push_back(outputpath.string());

	// cmout mask
	args.push_back("-om");
	args.push_back(std::to_string(request->cmoutmask()));

	// tmp dir
	args.push_back("-w");
	args.push_back(getTmpDirPath().string());

	// encoder type
	args.push_back("--encoder-type");
	args.push_back(encoderTypeToStr(request->encoder_type()));
	std::string encoder = request->encoder();
	if (encoder.size() > 0) {
		args.push_back("--encoder");
		args.push_back(encoder);
	}
	if (request->encoder_option().size() > 0) {
		args.push_back("--encoder-option");
		args.push_back(request->encoder_option());
	}

	// decoder
	args.push_back("--mpeg2decoder");
	args.push_back(decoderToStr(request->mpeg2_decoder()));

	args.push_back("--h264decoder");
	args.push_back(decoderToStr(request->mpeg2_decoder()));
	

	// enable chapter analysis
	if (!request->disable_chapter()) {
		args.push_back("--chapter");
		args.push_back("--jls-cmd");
		args.push_back((getJLDirPath() / "JL_標準.txt"));
	}

	// enable/disable delogo
	if (!request->disable_delogo()) {
		if (logopath_.count(request->service_id() % 100000) > 0) {
			args.push_back("--logo");
			args.push_back(logopath_[request->service_id() % 100000].string());
		} else {
			spdlog::warn("チャンネルID {0} に対するロゴが見つかりませんでした。ロゴ消しを無効化します。", request->service_id());
			args.push_back("--no-delogo");
		}
	}
	if (request->ignore_no_logo()) {
		args.push_back("--ignore-no-logo");
	}

	// enable/disable subtitles
	if (request->enable_subtitles()) {
		args.push_back("--subtitles");
	}
	if (request->ignore_no_drcsmap()) {
		args.push_back("ignore-no-drcsmap");
	}
}

int YukikazeServiceImpl::exec(const std::vector<std::string> & args) {
	char** argv = new char*[args.size()+1];
	const char* cmd = args[0].c_str();
	int fd;
	int status;

	pid_t pid = fork();
	switch (pid) {
	case -1:
		perror("fork()");
		throw(std::runtime_error("fork failed"));
		break;
	case 0:
		fd = open(logpath_.c_str(), O_CREAT|FD_CLOEXEC, 0600);
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
		waitpid(pid_, &status, 0);
		return WEXITSTATUS(status);
	}
	return -1;
}
