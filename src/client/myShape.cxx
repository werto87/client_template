#include "src/client/myShape.hxx"    // for MyShape
#include "src/client/webservice.hxx" // for Webservice
#include "src/common/pod.hxx"
#include <SFML/Graphics/CircleShape.hpp>  // for CircleShape
#include <SFML/Graphics/Color.hpp>        // for Color
#include <SFML/Graphics/Rect.hpp>         // for FloatRect
#include <SFML/Graphics/RenderWindow.hpp> // for RenderWindow
#include <SFML/Window/Mouse.hpp>          // for Mouse
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/execution/any_executor.hpp> // for any_executor
#include <boost/iterator/iterator_facade.hpp>    // for operator!=, iterato...
#include <boost/math/constants/constants.hpp>
#include <boost/range/adaptor/indexed.hpp> // for indexed_iterator
#include <coroutine>                       // for suspend_always, cor...
#include <cstdio>                          // for size_t
#include <exception>                       // for exception
#include <iostream>                        // for operator<<, endl
#include <magic_enum.hpp>
#include <math.h>
#include <utility> // for exchange

using boost::asio::awaitable;

float
directionToRotation (Direction direction)
{
  switch (direction)
    {
    case Direction::UpLeft:
      {
        return 0.875f;
      }
    case Direction::Up:
      {
        return 0.0f;
      }
    case Direction::UpRight:
      {
        return 0.125f;
      }
    case Direction::Left:
      {
        return 0.75f;
      }
    case Direction::Nothing:
      {
        return 0.f;
      }
    case Direction::Right:
      {
        return 0.25f;
      }
    case Direction::DownLeft:
      {
        return 0.625f;
      }
    case Direction::Down:
      {
        return 0.5f;
      }
    case Direction::DownRight:
      {
        return 0.375f;
      }
    default:
      return 0.0f;
    }
}

sf::VertexArray
openCircle (sf::Vector2f const &mid, float radius, sf::Color const &color, Direction direction, float open, int maxpts)
{

  sf::VertexArray arr (sf::TriangleFan);
  arr.append (mid);
  arr[0].color = color;
  auto maxptsAsFloat = static_cast<float> (maxpts);
  for (float i = 1; i < maxptsAsFloat; ++i)
    {
      const auto pi = boost::math::constants::pi<float> ();
      const float a = ((directionToRotation (direction) * 2.f + 0.5f) * pi - ((2.f * pi * open) / 2.f)) + (i * (2.f * pi * open)) / (maxptsAsFloat);
      arr.append (mid + radius * sf::Vector2f (std::cos (a), std::sin (a)));
      arr[static_cast<size_t> (i)].color = color;
    }
  arr.append (mid);
  arr[arr.getVertexCount () - 1].color = color;
  return arr;
}

sf::VertexArray
circle (sf::Vector2f const &mid, float radius, sf::Color const &color, int maxpts)
{
  sf::VertexArray arr (sf::LinesStrip);
  auto maxptsAsFloat = static_cast<float> (maxpts);
  for (float i = 0; i < maxptsAsFloat; ++i)
    {
      const auto pi = boost::math::constants::pi<float> ();
      arr.append (mid + radius * sf::Vector2f (std::cos ((2 * pi * i) / maxptsAsFloat), std::sin ((2 * pi * i) / (maxptsAsFloat - 1))));
      arr[static_cast<size_t> (i)].color = color;
    }
  return arr;
}

sf::Vector2f
offsetForAnimation (Direction direction)
{
  auto result = sf::Vector2f{};
  switch (direction)
    {
    case Direction::UpLeft:
      {
        result.y = -100;
        result.x = -100;
        break;
      }
    case Direction::Up:
      {
        result.y = -100;
        break;
      }
    case Direction::UpRight:
      {
        result.y = -100;
        result.x = 100;
        break;
      }
    case Direction::Left:
      {
        result.x = -100;
        break;
      }
    case Direction::Nothing:
      {
        break;
      }
    case Direction::Right:
      {
        result.x = 100;
        break;
      }
    case Direction::DownLeft:
      {
        result.y = 100;
        result.x = -100;
        break;
      }
    case Direction::Down:
      {
        result.y = 100;
        break;
      }
    case Direction::DownRight:
      {
        result.y = 100;
        result.x = 100;
        break;
      }
    default:
      break;
    }
  return result;
}

boost::posix_time::millisec
animationTime (Direction direction)
{
  auto result = boost::posix_time::millisec{ 0 };
  switch (direction)
    {
    case Direction::Left:
    case Direction::Up:
    case Direction::Right:
    case Direction::Down:
      {
        result = boost::posix_time::millisec{ 950 };
        break;
      }
    case Direction::DownLeft:
    case Direction::DownRight:
    case Direction::UpLeft:
    case Direction::UpRight:
      {
        result = boost::posix_time::millisec{ 1350 };
        break;
      }
    case Direction::Nothing:
      {
        break;
      }
    default:
      break;
    }
  return result;
}

void
MyShape::myFunction (boost::asio::io_context &_io_context, sf::RenderWindow &window, bool, bool, Webservice &webservice)
{
  shapes = std::vector<sf::VertexArray> (webservice.getResult ().size ());
  const auto sqrtOfSize = static_cast<unsigned long> (std::sqrt (shapes.size ()));
  using namespace boost::adaptors;
  // auto translated_pos = window.mapPixelToCoords (sf::Mouse::getPosition (window)); // Mouse position translated into world coordinates
  auto shapesToDraw = std::vector<sf::Vertex>{};
  players.clear ();
  for (auto const &shape : shapes | indexed (0))
    {
      auto const index = static_cast<size_t> (shape.index ());
      auto &shapeValue = shape.value ();
      auto x = static_cast<float> ((index % sqrtOfSize) * 100) + 50;
      auto y = static_cast<float> ((index / sqrtOfSize) * 100) + 50;
      if (webservice.getResult ().at (index).playerId.has_value ())
        {
          auto const &playerId = webservice.getResult ().at (index).playerId.value ();
          if (auto playerItr = std::find_if (webservice.getPlayers ().begin (), webservice.getPlayers ().end (), [&playerId] (auto const &player) { return player.playerId == playerId; }); playerItr != webservice.getPlayers ().end ())
            {
              std::cout << "index: " << index << "move direction: " << magic_enum::enum_name (playerItr->moveDirection) << std::endl;
              if (playerItr->moveDirection == Direction::Nothing)
                {

                  // index ist der alte und dann wird reset gemacht das darf aber nicht gemacht werden
                  // aufgabe herausfinden warum moveDirection nothing gesendet wird mit dem alten index

                  if (playerAnimationOffset.count (playerId) != 0) playerAnimationOffset.at (playerId).reset ();
                }
              else
                {
                  if (playerAnimationOffset.count (playerId) == 0) playerAnimationOffset.emplace (playerId, _io_context);
                  if (not playerAnimationOffset.at (playerId).isRunning)
                    {
                      std::cout << "co_spawn" << std::endl;
                      co_spawn (
                          _io_context,
                          [this, &playerId, direction = playerItr->moveDirection] () mutable {
                            return playerAnimationOffset.at (playerId).addToValueInStepsOverTime (sf::Vector2f{ 0, 0 }, offsetForAnimation (direction), 20, animationTime (direction));
                          },
                          boost::asio::detached);
                    }
                }
              auto animationOffset = sf::Vector2f{};
              if (playerAnimationOffset.count (playerId) != 0)
                {
                  animationOffset = playerAnimationOffset.at (playerId).value;
                }
              players.push_back (openCircle ({ x + animationOffset.x, y + animationOffset.y }, 40, playerItr->color, playerItr->viewDirection, playerItr->viewFieldsize, 100));
            }
        }
      shapeValue = circle ({ x, y }, 50, webservice.getResult ().at (index).color, 100);

      // if (shapeValue.getGlobalBounds ().contains (translated_pos)) // Rec  tangle-contains-point check
      //   {
      //     if (leftMouseButtonReleased)
      //       {
      //         try
      //           {
      //             co_await webservice.sendMsg ("next color|" + std::to_string (index));
      //           }
      //         catch (const std::exception &ex)
      //           {
      //             std::cerr << ex.what () << std::endl;
      //           }
      //       }
      //     if (rightMouseButtonReleased)
      //       {
      //         try
      //           {
      //             co_await webservice.sendMsg ("previous color|" + std::to_string (index));
      //           }
      //         catch (const std::exception &ex)
      //           {
      //             std::cerr << ex.what () << std::endl;
      //           }
      //       }
      //   }
      // shapeValue.setOutlineThickness (-20);
      // ;
      // webservice.getPlayerLooksInDirection ();
      // shapeValue.setFillColor (sf::Color::Red);
    }

  for (auto const &shape : shapes)
    {
      window.draw (shape);
    }
  for (auto const &player : players)
    {
      window.draw (player);
    }
}