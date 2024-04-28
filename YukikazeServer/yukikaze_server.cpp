#include <filesystem>
#include <regex>
#include <grpc/status.h>

#include "spdlog/spdlog.h"
#include "api/yukikaze.pb.h"
#include "api/yukikaze.grpc.pb.h"
#include "yukikaze_server.hpp"


yukikaze::YukikazeServiceImpl::YukikazeServiceImpl(const char* basepath, const char* recordedpath): basepath_(basepath), recordedpath_(recordedpath) {
	if (
		!std::filesystem::exists(getJLDirPath()) || 
		!std::filesystem::exists(getJLDirPath() / "JL_標準.txt")
	) {
		throw(std::runtime_error("No JL command files found!"));
	}

	std::filesystem::path logodir = (basepath_ / "logo");
	if (!std::filesystem::exists(logodir)) {
		spdlog::warn("no logo directory found\n");
		std::filesystem::create_directories(logodir);
	} else {
		// load logo files
		std::regex re(R"(SID(\d+)-.*.lgd)");
		std::smatch m;

		for (const std::filesystem::path& entry : std::filesystem::directory_iterator(logodir)) {
			std::string filename = entry.filename().string();
			if (std::regex_match(filename, m, re) && m.length() >= 2) {
				auto pair = std::make_pair(std::stoi(m[1]), std::filesystem::absolute(entry));
				logopath_.insert(pair);
				spdlog::info("loaded logo: {0}", filename.c_str());
			}
		}
	}

	std::filesystem::create_directories(getTmpDirPath());
	std::filesystem::create_directories(getLogDirPath());
}

yukikaze::YukikazeServiceImpl::~YukikazeServiceImpl () {
	delete encoder_.get();
	encoder_.release();
}


grpc::Status yukikaze::YukikazeServiceImpl::EncodeStatus(grpc::ServerContext* ctx, const yukikaze::StatusRequest* request, grpc::ServerWriter<StatusResponse>* stream) {
	spdlog::info("Status Request");
	yukikaze::StatusResponse resp;

	if (encoder_ == NULL && !isRunning_) {
		resp.set_log("no running jobs\n");
		stream->Write(resp);
		return grpc::Status::OK;
	}

	if (!isRunning_ && !hasFailed_ && encoder_ != NULL) {
		resp.set_log("successfully finished\n");
		stream->Write(resp);
		return grpc::Status::OK;
	}

	int fd = open(logpath_.c_str(), O_RDONLY);
	if (fd < 0) {
		return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "failed to open the log file");
	}

	char buf[512];
	if (hasFailed_) {
		int ret = 1;
		while (ret > 0) {
			ret = read(fd,buf,sizeof(buf));
			resp.set_log(std::string(buf, ret));
			stream->Write(resp);
		}
	}

	while (isRunning_) {
		int ret = read(fd, buf, sizeof(buf));
		if (ret > 0) {
			resp.set_log(std::string(buf, ret));
			stream->Write(resp);
		} else {
			sleep(1);
		}
	}
	close(fd);

	return grpc::Status::OK;
}

grpc::Status yukikaze::YukikazeServiceImpl::EncodeCancel(grpc::ServerContext* ctx, const yukikaze::CancelRequest* request, yukikaze::CancelResponse* reply) {
	spdlog::info("Cancel Request");

	if (!isRunning_) {
		return grpc::Status(grpc::StatusCode::OK, "no running jobs");
	}

	int ret = encoder_->cancel();
	if (ret != 0) {
		return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "exited at " + std::to_string(ret));
	}
	return grpc::Status::OK;
}