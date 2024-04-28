#ifndef YUKIKAZE_SERVER_H
#define YUKIKAZE_SERVER_H

#include <filesystem>
#include "api/yukikaze.grpc.pb.h"

namespace yukikaze {

class YukikazeServiceImpl final : public yukikaze::Encoder::Service {
public:
    YukikazeServiceImpl(const char* basepath, const char* recordedpath);
    ~YukikazeServiceImpl();

	grpc::Status Encode(grpc::ServerContext* ctx, const EncodeRequest* request, EncodeResponse* reply) override;

	grpc::Status EncodeStatus(grpc::ServerContext* ctx, const StatusRequest* request, grpc::ServerWriter<StatusResponse>* stream);

	grpc::Status EncodeCancel(grpc::ServerContext* ctx, const CancelRequest* request, CancelResponse* reply) override;
private:
	// fileds
	std::filesystem::path logpath_;
	std::filesystem::path basepath_;
	std::filesystem::path recordedpath_;
	std::map<int,std::filesystem::path> logopath_;
	pid_t pid_;
	bool isRunning_ = false;
	bool hasFailed_ = false;

	// methods
	void parseRequest(const EncodeRequest*, std::vector<std::string>&);
	std::string decoderToStr(Decoder);
	std::string encoderTypeToStr(EncoderType);
	int exec(const std::vector<std::string> & args);
	std::filesystem::path  getJLDirPath() {
		return basepath_ / "JL";
	}
	std::filesystem::path getTmpDirPath() {
		return basepath_ / "tmp";
	}
	std::filesystem::path getLogDirPath() {
		return basepath_ / "log";
	}
};

} // namespace yukikaze
#endif
