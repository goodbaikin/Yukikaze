#include "../include/CLI11.hpp"
#include "commands/cancel.cpp"
#include "commands/encode.cpp"
#include "commands/status.cpp"

int main(int argc, char** argv) {
	CLI::App app{"Yukikaze Client"};
	argv = app.ensure_utf8(argv);

	auto server = std::make_shared<std::string>();
	app.add_option("-s,--server", *server, "server address")->required();

	auto encodeCommand = app.add_subcommand("encode", "request for encoding");
	setupEncodeCommand(encodeCommand, server);
	auto cancelCommand = app.add_subcommand("cancel", "cancel for the running encode");
	setupCancelCommand(cancelCommand, server);
	auto statusCommand = app.add_subcommand("status", "print encode status to stdout");
	setupStatusCommand(statusCommand, server);

	app.require_subcommand();
	CLI11_PARSE(app, argc, argv);
	return 0;
}