#include <boost/asio/io_service.hpp>
#include <boost/system/error_code.hpp>

#include "common/config/config.h"
#include "common/log/log.h"

#include "core/command_line/standard/command_line.h"
#include "core/network_protocol.h"
#include "core/server/server.h"
#include "core/transport_virtual_layer_policies/transport_protocol_policy.h"

using Server =
    ssf::SSFServer<ssf::network::Protocol, ssf::TransportProtocolPolicy>;

int main(int argc, char** argv) {
  ssf::log::Configure();

  // The command line parser
  ssf::command_line::standard::CommandLine cmd(true);

  // Parse the command line
  boost::system::error_code ec;
  cmd.parse(argc, argv, ec);

  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "server: wrong arguments";
    return 1;
  }

  // Load SSF config if any
  ssf::Config ssf_config = ssf::LoadConfig(cmd.config_file(), ec);

  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "server: invalid config file format";
    return 1;
  }

  // Initiate and start the server
  Server server;

  // construct endpoint parameter stack
  auto endpoint_query = ssf::network::GenerateServerQuery(
      cmd.addr(), std::to_string(cmd.port()), ssf_config);

  server.Run(endpoint_query, ec);

  if (!ec) {
    BOOST_LOG_TRIVIAL(info) << "server: listening on port " << cmd.port();
    BOOST_LOG_TRIVIAL(info) << "server: press [ENTER] to stop";
    getchar();
  } else {
    BOOST_LOG_TRIVIAL(error)
        << "server: error happened when running server : " << ec.message();
  }

  BOOST_LOG_TRIVIAL(info) << "server: stop" << std::endl;
  server.Stop();

  return 0;
}