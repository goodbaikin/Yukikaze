#include <string>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "CLI11.hpp"
#include "api/yukikaze.pb.h"
#include "api/yukikaze.grpc.pb.h"
#include "spdlog/spdlog.h"
#include "yukikaze_server.hpp"

using namespace std;
using namespace grpc;
using namespace yukikaze;

void RunServer(const std::string & host, YukikazeServiceImpl & service) {

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(host, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << host << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}


int main(int argc, char** argv) {
  CLI::App app{"Yukikaze Server"};
  argv = app.ensure_utf8(argv);
  
  auto port = std::make_shared<int>(32768);
  app.add_option("-p,--port", *port, "listen port");
  auto address = std::make_shared<std::string>("0.0.0.0");
  app.add_option("-a,--address", *address, "listen address");

  auto basepath = std::make_shared<std::string>();
  app.add_option("basepath", *basepath, "path for data directory")->required();
  auto recordedPath = std::make_shared<std::string>();
  app.add_option("recordedpath", *recordedPath, "path for recorded directory")->required();

  CLI11_PARSE(app, argc, argv);
  spdlog::set_level(spdlog::level::debug);

  YukikazeServiceImpl service(basepath->c_str(), recordedPath->c_str());
	RunServer(*address+":"+to_string(*port), service);

	return 0;
}