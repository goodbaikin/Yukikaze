#pragma once
#include "../api/yukikaze.grpc.pb.h"

namespace yukikaze {

class YukikazeClient {
public:
	YukikazeClient(std::string target);
	void Encode(
		std::string input, 
		std::string output, 
		uint32_t serviceId,
		int cmoutmask=2,
		EncoderType encoderType=X264,
		std::string encoder="x264",
		std::string encoderOption="",
		bool disable_chapter=false,
		bool disable_delogo=false,
		bool ignore_no_logo=true,
		bool enable_subtitiles=false,
		bool ignore_no_drcsmap=false,
		Decoder mpeg2_decoder=DEFAULT,
		Decoder h264_decoder=DEFAULT
	);
	void EncodeStatus();
	void EncodeCancel();
private:
	std::unique_ptr<yukikaze::Encoder::Stub> stub_;
};

}