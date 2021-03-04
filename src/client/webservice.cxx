#include "src/common/pod.hxx"
#include <SFML/Graphics/Color.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/smart_ptr/make_unique.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/utility/string_view.hpp>
#include <chrono>
#include <iosfwd>
#include <magic_enum.hpp>
#include <map> // for map
#include <src/client/webservice.hxx>
#include <src/util/util.hxx>
#include <stddef.h>
#include <vector>
namespace boost
{
namespace asio
{
class io_context;
}
}
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace beast = boost::beast;         // from <bo   ost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

Webservice::Webservice (boost::asio::io_context &_io_context) : ws{ _io_context } {}

awaitable<void>
Webservice::connect ()
{

  // co_await ws.async_connect ({ tcp::v4 (), 55555 }, use_awaitable);

  // beast::error_code ec;

  // These objects perform our I/O
  tcp::resolver resolver (ws.get_executor ());

  // Look up the domain name
  //   auto const results = resolv  er  .async_resolve (host, port,  yield[ec]);
  auto const results = co_await resolver.async_resolve ({ tcp::v4 (), 55555 }, use_awaitable);
  // if (ec) return fail (ec, "resolve");

  // Set a timeout on the operation
  beast::get_lowest_layer (ws).expires_after (std::chrono::seconds (30));

  // Make the connection on the IP address we get from a lookup
  auto ep = co_await beast::get_lowest_layer (ws).async_connect (results, use_awaitable);
  // if (ec) return fail (ec, "connect");

  // Update the host_ string. This will provide the value of the
  // Host HTTP header during the WebSocket handshake.
  // See https://tools.ietf.org/html/rfc7230#section-5.4
  std::string host = "localhost:" + std::to_string (ep.port ());

  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  beast::get_lowest_layer (ws).expires_never ();

  // Set suggested timeout settings for the websocket
  ws.set_option (websocket::stream_base::timeout::suggested (beast::role_type::client));

  // Set a decorator to change the User-Agent of the handshake
  ws.set_option (websocket::stream_base::decorator ([] (websocket::request_type &req) { req.set (http::field::user_agent, std::string (BOOST_BEAST_VERSION_STRING) + " websocket-client-coro"); }));

  // Perform the websocket handshake
  co_await ws.async_handshake (host, "/", use_awaitable);
}

void
Webservice::start ()
{
  if (not shouldSendRequest)
    {
      shouldSendRequest = true;
      co_spawn (ws.get_executor (), boost::bind (&Webservice::updateBoard, this), detached);
    }
}

void
Webservice::stop ()
{
  shouldSendRequest = false;
}

awaitable<void>
Webservice::updateBoard ()
{

  while (shouldSendRequest)
    {
      beast::flat_buffer buffer;

      co_await ws.async_read (buffer, use_awaitable);
      auto msgRead = boost::beast::buffers_to_string (buffer.data ());
      std::cout << msgRead << std::endl;
      if (boost::algorithm::starts_with (msgRead, "game state###"))
        {
          std::vector<std::string> msgParts{};
          boost::algorithm::split (msgParts, msgRead, boost::is_any_of ("###"));
          for (auto &&msgPart : msgParts)
            {
              // std::cout << msgPart << std::endl;
              if (boost::algorithm::starts_with (msgPart, "players|"))
                {
                  auto playerStringStream = std::stringstream{};
                  std::vector<std::string> result{};
                  boost::algorithm::split (result, msgPart, boost::is_any_of ("|"));
                  // std::cout << "split string: " << std::endl;
                  // std::cout << "size: " << result.size () << std::endl;
                  // std::cout << result.at (0) << std::endl;
                  // std::cout << result.at (1) << std::endl;
                  // std::cout << result.at (2) << std::endl;
                  playerStringStream << result.at (2);
                  boost::archive::text_iarchive ia (playerStringStream);
                  players = std::vector<data::Player> (std::stoull (result.at (1).data ()));
                  for (auto &&player : players)
                    {
                      ia >> player;
                    }
                }
              if (boost::algorithm::starts_with (msgPart, "board|"))
                {
                  auto boardStringStream = std::stringstream{};
                  std::vector<std::string> result{};
                  boost::algorithm::split (result, msgPart, boost::is_any_of ("|"));
                  // std::cout << "split string: " << std::endl;
                  // std::cout << "size: " << result.size () << std::endl;
                  // std::cout << result.at (0) << std::endl;
                  // std::cout << result.at (1) << std::endl;
                  // std::cout << result.at (2) << std::endl;
                  boardStringStream << result.at (2);
                  boost::archive::text_iarchive ia (boardStringStream);
                  myBoard = std::vector<data::BoardElement> (std::stoull (result.at (1).data ()));
                  for (auto &&boardElement : myBoard)
                    {
                      ia >> boardElement;
                    }
                }
            }
        }
      if (boost::algorithm::starts_with (msgRead, "player id|"))
        {
          // read and set player number
          std::cout << "msg: " << msgRead << std::endl;
          std::vector<std::string> result{};
          boost::algorithm::split (result, msgRead, boost::is_any_of ("|"));
          std::cout << "split string: " << std::endl;
          std::cout << "size: " << result.size () << std::endl;
          std::cout << result.at (0) << std::endl;
          std::cout << result.at (1) << std::endl;
          playerId = result.at (1).data ();
        }
      if (boost::algorithm::starts_with (msgRead, "account id|"))
        {
          // read and set connection id
          std::cout << "msg: " << msgRead << std::endl;
          std::vector<std::string> result{};
          boost::algorithm::split (result, msgRead, boost::is_any_of ("|"));
          std::cout << "split string: " << std::endl;
          std::cout << "size: " << result.size () << std::endl;
          std::cout << result.at (0) << std::endl;
          std::cout << result.at (1) << std::endl;
          playerId = result.at (1).data ();
        }
    }
}

awaitable<void>
Webservice::sendMsg (std::string msg)
{
  co_await ws.async_write (boost::asio::buffer (msg), use_awaitable);
}

std::vector<data::BoardElement> const &
Webservice::getResult () const
{
  return myBoard;
}

std::string const &
Webservice::getPlayerId () const
{
  return playerId;
}

std::vector<data::Player> const &
Webservice::getPlayers () const
{
  return players;
}
