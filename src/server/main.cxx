#include "src/common/database.hxx"
#include "src/server/constant.hxx"
#include "src/server/server.hxx"
#include <boost/bind.hpp>
int
main ()
{
  try
    {
      dropTables (constant::pathToDb);
      createTables (constant::pathToDb);
      boost::asio::io_context io_context (1);
      boost::asio::signal_set signals (io_context, SIGINT, SIGTERM);
      signals.async_wait ([&] (auto, auto) { io_context.stop (); });
      auto const boardSize = size_t{ 49 };
      auto const costToMoveOneElementHorizontalOrVertical = 1;
      auto server = Server{ io_context, boardSize, costToMoveOneElementHorizontalOrVertical };
      co_spawn (io_context, boost::bind (&Server::listener, boost::ref (server)), boost::asio::detached);
      io_context.run ();
    }
  catch (std::exception &e)
    {
      std::printf ("Exception: %s\n", e.what ());
    }
}