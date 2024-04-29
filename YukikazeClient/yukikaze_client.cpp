#include <grpcpp/grpcpp.h>

#include "yukikaze_client.hpp"

using namespace yukikaze;

YukikazeClient::YukikazeClient(std::string target) {
	std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
	stub_ = Encoder::NewStub(channel);
}

void YukikazeClient::Encode(
	std::string input,
	std::string output,
	uint32_t serviceId,
	int cmoutmask,
	EncoderType encoderType,
	std::string encoder,
	std::string encoderOption,
	bool disable_chapter,
	bool disable_delogo,
	bool ignore_no_logo,
	bool enable_subtitiles,
	bool ignore_no_drcsmap,
	Decoder mpeg2_decoder,
	Decoder h264_decoder
) {
	EncodeRequest request;
	request.set_input_name(input);
	request.set_output_name(output);
	request.set_service_id(serviceId);
	request.set_cmoutmask(cmoutmask);
	request.set_encoder_type(encoderType);
	request.set_encoder(encoder);
	request.set_encoder_option(encoderOption);
	request.set_disable_chapter(disable_chapter);
	request.set_disable_delogo(disable_delogo);
	request.set_ignore_no_logo(ignore_no_logo);
	request.set_enable_subtitles(enable_subtitiles);
	request.set_ignore_no_drcsmap(ignore_no_drcsmap);
	request.set_mpeg2_decoder(mpeg2_decoder);
	request.set_h264_decoder(h264_decoder);

	grpc::ClientContext ctx;
	ctx.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(1));
	EncodeResponse resp;
	grpc::Status status = stub_->Encode(&ctx, request, &resp);
	if (status.ok()) {
		std::cout << "successfully encoded" << std::endl;
		return;
	}
}

void YukikazeClient::EncodeStatus() {
	grpc::ClientContext ctx;
	StatusRequest request;
	auto stream = stub_->EncodeStatus(&ctx, request);
	
	StatusResponse resp;
	while (stream->Read(&resp)) {
		std::cout << resp.log();
	}
}

void YukikazeClient::EncodeCancel() {
	grpc::ClientContext ctx;
	CancelRequest request;
	CancelResponse response;
	
	grpc::Status status = stub_->EncodeCancel(&ctx, request, &response);

	if (status.ok()) {
		std::cout << "successfully canceled" << std::endl;
	} else {
		std::cout << "error occured while canceling" << std::endl;
		throw(std::runtime_error(status.error_message()));
	}
}