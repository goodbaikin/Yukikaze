#include "../../include/CLI11.hpp"
#include "../yukikaze_client.hpp"

void cancel(std::string const& server) {
	yukikaze::YukikazeClient client(server);
	client.EncodeCancel();
}

void setupCancelCommand(CLI::App *app, std::shared_ptr<std::string> server) {
	app->callback([server] {cancel(*server);});
}