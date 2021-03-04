#include "src/client/webservice.hxx"
#include <SFML/Graphics/RenderWindow.hpp>      // for RenderWindow
#include <SFML/System/Clock.hpp>               // for Clock
#include <SFML/Window/Event.hpp>               // for Event, Even...
#include <SFML/Window/Mouse.hpp>               // for Mouse, Mous...
#include <SFML/Window/VideoMode.hpp>           // for VideoMode
#include <boost/asio/associated_allocator.hpp> // for get_associa...
#include <boost/asio/async_result.hpp>         // for async_initiate
#include <boost/asio/awaitable.hpp>            // for awaitable
#include <boost/asio/basic_waitable_timer.hpp> // for basic_waita...
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>                       // for detached
#include <boost/asio/detail/impl/epoll_reactor.hpp>      // for epoll_react...
#include <boost/asio/detail/impl/service_registry.hpp>   // for service_reg...
#include <boost/asio/detail/impl/signal_set_service.ipp> // for signal_set_...
#include <boost/asio/execution/any_executor.hpp>         // for any_executor
#include <boost/asio/execution/context_as.hpp>           // for query
#include <boost/asio/execution/prefer_only.hpp>          // for prefer, query
#include <boost/asio/io_context.hpp>                     // for io_context
#include <boost/asio/signal_set.hpp>                     // for signal_set
#include <boost/asio/steady_timer.hpp>                   // for steady_timer
#include <boost/asio/this_coro.hpp>                      // for executor
#include <boost/asio/use_awaitable.hpp>                  // for use_awaitable
#include <boost/bind/bind.hpp>
#include <chrono>       // for operator""ms
#include <coroutine>    // for coroutine_h...
#include <cstdio>       // for printf
#include <exception>    // for exception
#include <imgui-SFML.h> // for Init, Proce...
#include <imgui.h>      // for Button, Begin
#include <iostream>     // for operator<<
#include <memory>       // for operator==
#include <new>          // for operator new
#include <ratio>        // for ratio
#include <signal.h>     // for SIGINT, SIG...
#include <string>
#include <type_traits> // for m ove, addre...
#include <utility>     // for exchange

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;

awaitable<void>
ui (boost::asio::io_context &io_context)
{
  char textBox[50];
  sf::RenderWindow window (sf::VideoMode (640, 480), "ImGui + SFML = <3");
  window.setFramerateLimit (60);
  ImGui::SFML::Init (window);
  auto webservice = Webservice{ io_context };
  co_await webservice.connect ();
  sf::Clock deltaClock;
  while (window.isOpen ())
    {
      sf::Event event;
      while (window.pollEvent (event))
        {
          ImGui::SFML::ProcessEvent (event);

          if (event.type == sf::Event::Closed)
            {
              window.close ();
            }
        }

      auto frameDeltaTime = deltaClock.restart ();
      ImGui::SFML::Update (window, frameDeltaTime);
      ImGui::Begin ("Hello, world!");
      if (ImGui::Button ("Send Message"))
        {
          co_await webservice.sendMsg (textBox);
        }
      ImGui::InputText ("msg", textBox, sizeof (textBox));
      ImGui::End ();
      window.clear ();
      ImGui::SFML::Render (window);
      window.display ();
      auto timer = boost::asio::steady_timer (co_await this_coro::executor);
      using namespace std::chrono_literals;
      timer.expires_after (10ms);
      co_await timer.async_wait (use_awaitable);
    }
  ImGui::SFML::Shutdown ();
  io_context.stop ();
}

int
main ()
{
  try
    {
      boost::asio::io_context io_context (1);
      boost::asio::signal_set signals (io_context, SIGINT, SIGTERM);
      signals.async_wait ([&] (auto, auto) { io_context.stop (); });
      co_spawn (io_context, boost::bind (ui, std::ref (io_context)), detached);
      io_context.run ();
    }
  catch (std::exception &e)
    {
      std::printf ("Exception: %s\n", e.what ());
    }
}