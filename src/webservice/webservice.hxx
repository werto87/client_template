#ifndef C5B152C0_B968_4BBC_B1F9_5823AA8FAD87
#define C5B152C0_B968_4BBC_B1F9_5823AA8FAD87
#include "src/util/util.hxx"
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace boost
{
namespace asio
{
class io_context;
}
}
class Webservice
{
public:
  Webservice (boost::asio::io_context &_io_context);

  boost::asio::awaitable<void> connect ();

  boost::asio::awaitable<void> sendMsg (std::string msg);

  boost::asio::awaitable<void> read ();

private:
  boost::asio::awaitable<std::string> my_read ();

  boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
  std::vector<std::string> msgToSend{};
};
#endif /* C5B152C0_B968_4BBC_B1F9_5823AA8FAD87 */
