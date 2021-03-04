#include "src/common/pod.hxx"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/type_index.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <chrono>
#include <cmath>
#include <exception>
#include <iostream>
#include <istream>
#include <iterator>
#include <magic_enum.hpp>
#include <ostream>
#include <soci/boost-fusion.h>
#include <soci/boost-gregorian-date.h>
#include <soci/boost-optional.h>
#include <soci/boost-tuple.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <src/server/server.hxx>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
using namespace boost::beast;
using namespace boost::asio;

Server::Server (boost::asio::io_context &io_context, size_t boardSize, int costToMoveOneElementHorizontalOrVertical) : _io_context{ io_context }, board (boardSize, costToMoveOneElementHorizontalOrVertical) {}

awaitable<std::string>
Server::my_read (websocket::stream<tcp_stream> &ws_)
{
  std::cout << "read" << std::endl;
  flat_buffer buffer;
  co_await ws_.async_read (buffer, use_awaitable);
  auto msg = buffers_to_string (buffer.data ());
  std::cout << "number of letters '" << msg.size () << "' msg: '" << msg << "'" << std::endl;
  co_return msg;
}

awaitable<void>
Server::handleMessage (std::shared_ptr<websocket::stream<tcp_stream> > ws_, std::string const &msg)
{
  std::cout << "handleMessage: " << msg << std::endl;
  if (boost::algorithm::contains (msg, "player direction|"))
    {
      std::vector<std::string> result{};
      boost::algorithm::split (result, msg, boost::is_any_of ("|"));
      std::cout << "split string: " << std::endl;
      std::cout << "size: " << result.size () << std::endl;
      std::cout << result.at (0) << std::endl;
      std::cout << result.at (1) << std::endl;
      boost::algorithm::split (result, result.at (1), boost::is_any_of (","));
      std::cout << "player id: " << result.at (0) << std::endl;
      std::cout << "direction: " << result.at (1) << std::endl;
      auto playerId = result.at (0);
      auto direction = *magic_enum::enum_cast<Direction> (result.at (1));
      board.delayMoveByCostOfMovement (playerId, players, direction, playerMoveTimer.at (playerId)); // passing player id and players vector instead of player we gone invoke some functions later and iterator could be not valid anymore
    }
  if (boost::algorithm::starts_with (msg, "create new player|"))
    {
      std::cout << "msg: " << msg << std::endl;
      createPlayer (sf::Color::Red);
      co_await ws_->async_write (boost::asio::buffer ("player id|" + players.back ().playerId), use_awaitable);
    }
}

awaitable<void>
Server::readFromClient (std::shared_ptr<websocket::stream<tcp_stream> > ws_)
{
  try
    {
      for (;;)
        {
          auto readResult = co_await my_read (*ws_);
          co_await handleMessage (ws_, readResult);
        }
    }
  catch (std::exception &e)
    {
      std::cout << "echo  Exception: " << e.what () << std::endl;
    }
}

awaitable<void>
Server::writeToClient (std::shared_ptr<websocket::stream<tcp_stream> > ws_)
{
  try
    {
      for (;;)
        {
          auto timer = steady_timer (co_await this_coro::executor);
          using namespace std::chrono_literals;
          timer.expires_after (14ms);
          co_await timer.async_wait (use_awaitable);
          auto playerStringStream = std::stringstream{};
          boost::archive::text_oarchive playerArchive{ playerStringStream };
          for (auto &&player : players)
            {
              playerArchive << player;
            }
          auto boardStringStream = std::stringstream{};
          boost::archive::text_oarchive boardArchive{ boardStringStream };
          for (auto &&boardElement : board.myBoard)
            {
              boardArchive << boardElement;
            }
          co_await ws_->async_write (buffer ("game state###players|" + std::to_string (players.size ()) + "|" + playerStringStream.str () + "###board|" + std::to_string (board.myBoard.size ()) + "|" + boardStringStream.str ()), use_awaitable);
        }
    }
  catch (std::exception &e)
    {
      std::printf ("echo Exception:  %s\n", e.what ());
    }
}

void
Server::createAccount (sf::Color const &color)
{
  static boost::uuids::random_generator generator;
  boost::lexical_cast<std::string> (generator ());
}

void
Server::createPlayer (sf::Color const &color)
{
  static boost::uuids::random_generator generator;
  players.emplace_back (data::Player{ boost::lexical_cast<std::string> (generator ()), Direction::Up, Direction::Nothing, 0.95, color });
  board.myBoard.at (board.firstEmptyPlaceOnBoard ()).playerId = players.back ().playerId;
  playerMoveTimer.emplace (players.back ().playerId, boost::asio::deadline_timer{ _io_context });
}

awaitable<void>
Server::listener ()
{
  auto executor = co_await this_coro::executor;
  ip::tcp::acceptor acceptor (executor, { ip::tcp::v4 (), 55555 });
  for (;;)
    {
      ip::tcp::socket socket = co_await acceptor.async_accept (use_awaitable);
      auto ws_ = std::make_shared<websocket::stream<tcp_stream> > (std::move (socket));
      ws_->set_option (websocket::stream_base::timeout::suggested (role_type::server));
      ws_->set_option (websocket::stream_base::decorator ([] (websocket::response_type &res) { res.set (http::field::server, std::string (BOOST_BEAST_VERSION_STRING) + " websocket-server-async"); }));
      co_await ws_->async_accept (use_awaitable);

      static boost::uuids::random_generator generator;
      co_await ws_->async_write (boost::asio::buffer ("account id|" + boost::lexical_cast<std::string> (generator ())), use_awaitable);
      std::cout << "finished send player id" << std::endl;
      co_spawn (
          executor, [this, ws_] () mutable { return readFromClient (ws_); }, detached);
      co_spawn (
          executor, [this, ws_] () mutable { return writeToClient (ws_); }, detached);
    }
}
