#ifndef C5B152C0_B968_4BBC_B1F9_5823AA8FAD87
#define C5B152C0_B968_4BBC_B1F9_5823AA8FAD87
#include "src/common/pod.hxx"
#include "src/util/util.hxx"
#include <SFML/Graphics/Color.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr/make_unique.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/utility/string_view.hpp>
#include <chrono>
#include <iosfwd>
#include <iostream>
#include <map> // for m  ap
#include <span>
#include <stddef.h>
#include <vector>

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

  void start ();

  void stop ();

  boost::asio::awaitable<void> updateBoard ();

  boost::asio::awaitable<void> sendMsg (std::string msg);

  std::vector<data::BoardElement> const &getResult () const;

  std::string const &getPlayerId () const;

  int getPlayerLooksInDirection () const;

  std::vector<data::Player> const &getPlayers () const;

private:
  boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
  std::vector<data::BoardElement> myBoard{};
  bool shouldSendRequest = false;
  std::string playerId{};
  std::vector<data::Player> players{}; // all players the player can see including him self
  Direction playerLooksInDirection = Direction::Up;
};
#endif /* C5B152C0_B968_4BBC_B1F9_5823AA8FAD87 */
