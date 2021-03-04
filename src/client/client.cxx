#include "src/client/webservice.hxx"           // for Webservice
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
#include <boost/bind/bind.hpp>                           // for bind_t, lis...
#include <chrono>                                        // for operator""ms
#include <coroutine>                                     // for coroutine_h...
#include <cstdio>                                        // for printf
#include <exception>                                     // for exception
#include <imgui-SFML.h>                                  // for Init, Proce...
#include <imgui.h>                                       // for Button, Begin
#include <iostream>                                      // for operator<<
#include <memory>                                        // for operator==
#include <new>                                           // for operator new
#include <ratio>                                         // for ratio
#include <signal.h>                                      // for SIGINT, SIG...
#include <src/client/myShape.hxx>                        // for MyShape
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
  auto webservice = Webservice{ io_context };
  co_await webservice.connect ();
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window (sf::VideoMode (800, 600), "SFML shapes", sf::Style::Default, settings);
  window.setFramerateLimit (60);
  ImGui::SFML::Init (window);
  auto shapes = MyShape{};
  sf::Clock deltaClock;
  while (window.isOpen ())
    {
      // auto leftMouseButtonReleased = false;
      // auto rightMouseButtonReleased = false;
      sf::Event event;
      while (window.pollEvent (event))
        {
          // use UpLeft instead of Up and than left
          ImGui::SFML::ProcessEvent (event);
          switch (event.type)
            {
            case sf::Event::Closed:
              {
                window.close ();
                break;
              }
            // case sf::Event::MouseButtonReleased:
            //   {
            //     if (event.mouseButton.button == sf::Mouse::Left)
            //       {
            //         std::cout << "left mouse button released" << std::endl;
            //         leftMouseButtonReleased = true;
            //         break;
            //       }
            //     if (event.mouseButton.button == sf::Mouse::Right)
            //       {
            //         std::cout << "right mouse button released" << std::endl;
            //         rightMouseButtonReleased = true;
            //         break;
            //       }
            //   }
            case sf::Event::KeyPressed:
              {
                std::cout << "KeyPressed: " << event.key.code << std::endl;

                if (event.key.code == 82)
                  {
                    std::cout << "Num7: " << std::endl;
                    co_await webservice.sendMsg ("player direction|" + webservice.getPlayerId () + std::string{ "," } + "UpLeft");
                  }
                if (event.key.code == 83)
                  {
                    std::cout << "Num8: " << std::endl;
                    co_await webservice.sendMsg ("player direction|" + webservice.getPlayerId () + std::string{ "," } + "Up");
                  }
                if (event.key.code == 84)
                  {
                    std::cout << "Num9: " << std::endl;
                    co_await webservice.sendMsg ("player direction|" + webservice.getPlayerId () + std::string{ "," } + "UpRight");
                  }
                if (event.key.code == 79)
                  {
                    std::cout << "Num4: " << std::endl;
                    co_await webservice.sendMsg ("player direction|" + webservice.getPlayerId () + std::string{ "," } + "Left");
                  }
                if (event.key.code == 80)
                  {
                    std::cout << "Num5: " << std::endl;
                  }
                if (event.key.code == 81)
                  {
                    std::cout << "Num6: " << std::endl;
                    co_await webservice.sendMsg ("player direction|" + webservice.getPlayerId () + std::string{ "," } + "Right");
                  }
                if (event.key.code == 76)
                  {
                    std::cout << "Num1: " << std::endl;
                    co_await webservice.sendMsg ("player direction|" + webservice.getPlayerId () + std::string{ "," } + "DownLeft");
                  }
                if (event.key.code == 77)
                  {
                    std::cout << "Num2: " << std::endl;
                    co_await webservice.sendMsg ("player direction|" + webservice.getPlayerId () + std::string{ "," } + "Down");
                  }
                if (event.key.code == 78)
                  {
                    std::cout << "Num3: " << std::endl;
                    co_await webservice.sendMsg ("player direction|" + webservice.getPlayerId () + std::string{ "," } + "DownRight");
                  }
                break;
              }
            }
        }
      auto frameDeltaTime = deltaClock.restart ();
      ImGui::SFML::Update (window, frameDeltaTime);
      ImGui::Begin ("Hello, world!");
      if (ImGui::Button ("Start Send Requests"))
        {
          webservice.start ();
        }
      if (ImGui::Button ("Stop Send Requests"))
        {
          webservice.stop ();
        }
      if (ImGui::Button ("Send Message"))
        {
          co_await webservice.sendMsg (textBox);
        }
      if (ImGui::Button ("Create new Player"))
        {
          co_await webservice.sendMsg ("create new player|");
        }
      ImGui::InputText ("msg", textBox, sizeof (textBox));
      ImGui::End ();
      window.clear ();
      shapes.myFunction (io_context, window, /*leftMouseButtonReleased*/ false, /*rightMouseButtonReleased*/ false, webservice);
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