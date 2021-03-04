#ifndef B432D215_1879_44BF_B57C_8C3DB8289B94
#define B432D215_1879_44BF_B57C_8C3DB8289B94

#include "src/server/model.hxx"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <iostream>
#include <magic_enum.hpp>
#include <memory>
#include <span>

class Server
{
public:
  Server (boost::asio::io_context &_io_context, size_t boardSize, int costToMoveOneElementHorizontalOrVertical);

  size_t firstEmptyPlaceOnBoard ();

  boost::asio::awaitable<std::string> my_read (boost::beast::websocket::stream<boost::beast::tcp_stream> &ws_);

  boost::asio::awaitable<void> handleMessage (std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream> > ws_, std::string const &msg);

  boost::asio::awaitable<void> readFromClient (std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream> > ws_);

  boost::asio::awaitable<void> writeToClient (std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream> > ws_);

  boost::asio::awaitable<void> listener ();

  void createAccount (sf::Color const &color);

  void createPlayer (sf::Color const &color);

  std::map<std::string, boost::asio::deadline_timer> playerMoveTimer;
  boost::asio::io_context &_io_context;
  Board board;
  std::vector<data::Player> players{};
};
#endif /* B432D215_1879_44BF_B57C_8C3DB8289B94 */
