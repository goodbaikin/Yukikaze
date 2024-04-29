#include "../../include/CLI11.hpp"
#include "../yukikaze_client.hpp"

using namespace yukikaze;

struct encodeOpt {
	std::shared_ptr<std::string> server;
	std::string input;
	std::string output;
	uint32_t serviceId;
	int cmoutmask = 2;
	std::string encoderType = "x264";
	std::string encoder = "x264";
	std::string encoderOption;
	bool disable_chapter = false;
	bool disable_delogo = false;
	bool ignore_no_logo = true;
	bool enable_subtitles = false;
	bool ignore_no_drcsmap = false;
	std::string mpeg2_decoder = "default";
	std::string h264_decoder = "default";
};

EncoderType encoderTypeFromString(std::string encoderType) {
	if (encoderType == "x264") {
		return X264;
	} else if (encoderType == "x265") {
		return X265;
	} else if (encoderType == "QSVEnc") {
		return QSVENC;
	} else if (encoderType == "NVEnc") {
		return NVENC;
	} else if (encoderType == "VCEEnc") {
		return VCEENC;
	} else if (encoderType == "SVT-AV1") {
		return SVT_AV1;
	} else {
		throw(std::runtime_error("unknown encoderType"));
	}

	return X264;
}

Decoder decoderFromString(std::string decoder) {
	if (decoder == "default") {
		return DEFAULT;
	} else if (decoder == "QSV") {
		return QSV;
	} else if (decoder == "CUVID") {
		return CUVID;
	} else {
		throw(std::runtime_error("unknown decoder"));
	}
	return DEFAULT;
}

void encode(encodeOpt const& opt) {
	YukikazeClient client(*opt.server);
	client.Encode(
		opt.input,
		opt.output,
		opt.serviceId,
		opt.cmoutmask,
		encoderTypeFromString(opt.encoderType),
		opt.encoder,
		opt.encoderOption,
		opt.disable_chapter,
		opt.disable_delogo,
		opt.ignore_no_logo,
		opt.enable_subtitles,
		opt.ignore_no_drcsmap,
		decoderFromString(opt.mpeg2_decoder),
		decoderFromString(opt.h264_decoder)
	);
}

void setupEncodeCommand(CLI::App *app, std::shared_ptr<std::string> server) {
	auto opt = std::make_shared<encodeOpt>();
	opt->server = server;
	app->add_option("input", opt->input, "入力ファイルパス")->required();
	app->add_option("output", opt->output, "出力ファイルパス")->required();
	app->add_option("serviceId", opt->serviceId, "処理するサービスIDを指定")->required();

	app->add_option("--cmoutmask", opt->cmoutmask, "出力マスク[2]");
	app->add_option("--encoder-type", opt->encoderType, "対応エンコーダ: [x264],x265,QSVEnc,NVEnc,VCEEnc,SVT-AV");
	app->add_option("-e,--encoder", opt->encoder, "エンコーダへのパス[x264]");
	app->add_option("--encoder-option", opt->encoderOption, "エンコーダに渡す引数");
	app->add_option("--disable-chapter", opt->disable_chapter, "チャプター・CM解析を行わない");
	app->add_option("--disable-delogo", opt->disable_delogo, "ロゴ消しをしない");
	app->add_option("--ignore-no-logo", opt->ignore_no_logo, "ロゴが見つからなくても処理を続行する");
	app->add_option("--enable-subtitles", opt->enable_subtitles, "字幕を処理する");
	app->add_option("--ignore-no-drcsmap", opt->ignore_no_drcsmap, "マッピングにないDRCS外字があっても処理を続行する");
	app->add_option("--mpeg2-decoder", opt->mpeg2_decoder, "使用可能デコーダ: [default],QSV,CUVID");
	app->add_option("--h264-decoder", opt->h264_decoder, "使用可能デコーダ: [default],QSV,CUVID");

	app->callback([opt]() {encode(*opt);} );
}