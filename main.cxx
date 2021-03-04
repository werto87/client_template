#include "src/webservice/webservice.hxx"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <boost/bind/bind.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

boost::asio::awaitable<void>
ui (boost::asio::io_context &io_context)
{
  char textBox[50];
  sf::RenderWindow window (sf::VideoMode (640, 480), "ImGui + SFML = <3");
  window.setFramerateLimit (60);
  ImGui::SFML::Init (window);
  auto webservice = Webservice{ io_context };
  co_await webservice.connect ();
  co_spawn (
      io_context, [&] () mutable { return webservice.read (); }, boost::asio::detached);
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
      auto timer = boost::asio::steady_timer (co_await boost::asio::this_coro::executor);
      using namespace std::chrono_literals;
      timer.expires_after (10ms);
      co_await timer.async_wait (boost::asio::use_awaitable);
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
      boost::asio::co_spawn (io_context, boost::bind (ui, std::ref (io_context)), boost::asio::detached);
      io_context.run ();
    }
  catch (std::exception &e)
    {
      std::printf ("Exception: %s\n", e.what ());
    }
}