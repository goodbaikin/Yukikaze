#include "../../include/CLI11.hpp"
#include "../yukikaze_client.hpp"

void status(std::string const& server) {
	yukikaze::YukikazeClient client(server);
	client.EncodeStatus();
}

void setupStatusCommand(CLI::App *app, std::shared_ptr<std::string> server) {
	app->callback([server] {status(*server);});
}