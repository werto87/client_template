#include "src/server/model.hxx"
#include "src/common/pod.hxx"
#include <algorithm> // for find
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <cmath>
#include <iostream>       // for operator<<, endl, basic_ostream, cout, ost...
#include <iterator>       // for next, prev
#include <magic_enum.hpp> // for enum_name
#include <memory>         // for __to_address
#include <span>           // for span
#include <stdexcept>      // for out_of_range
#include <type_traits>    // for forward, enable_if<>::type, iter_swap

Board::Board (size_t boardSize, int _costToMoveOneElementHorizontalOrVertical) : myBoard (boardSize), costToMoveOneElementHorizontalOrVertical{ _costToMoveOneElementHorizontalOrVertical } {}

bool
Board::isAllowedToMoveUp (std::string const &playerId) const
{
  return playerPositionAsIndex (playerId) >= static_cast<size_t> (std::sqrt (myBoard.size ()));
}
bool
Board::isAllowedToMoveLeft (std::string const &playerId) const
{
  return not((playerPositionAsIndex (playerId) % static_cast<size_t> (std::sqrt (myBoard.size ()))) == 0);
}
bool
Board::isAllowedToMoveRight (std::string const &playerId) const
{
  return not((playerPositionAsIndex (playerId) % static_cast<size_t> (std::sqrt (myBoard.size ()))) == static_cast<size_t> (std::sqrt (myBoard.size ()) - 1));
}
bool
Board::isAllowedToMoveDown (std::string const &playerId) const
{
  return (myBoard.size () - static_cast<size_t> (std::sqrt (myBoard.size ()))) > playerPositionAsIndex (playerId);
}

auto
Board::playerPositionItr (std::string const &playerId)
{
  auto possiblePosition = std::find_if (myBoard.begin (), myBoard.end (), [&playerId] (data::BoardElement const &boardElement) { return boardElement.playerId.has_value () && playerId == boardElement.playerId.value (); });
  if (possiblePosition == myBoard.end ()) throw std::out_of_range{ "could not find playerId: " + playerId + "  in board array" };
  return possiblePosition;
}

size_t
Board::playerPositionAsIndex (std::string const &playerId) const
{
  auto possiblePosition = std::find_if (myBoard.begin (), myBoard.end (), [&playerId] (data::BoardElement const &boardElement) { return boardElement.playerId.has_value () && playerId == boardElement.playerId.value (); });
  if (possiblePosition == myBoard.end ()) throw std::out_of_range{ "could not find playerId: " + playerId + "  in board array" };
  return static_cast<size_t> (possiblePosition - myBoard.begin ());
}

bool
Board::moveUpLeft (std::string const &playerId)
{
  std::cout << "try moveUpLeft" << std::endl;
  if (isAllowedToMoveUp (playerId) && isAllowedToMoveLeft (playerId))
    {
      std::cout << "moveUpLeft" << std::endl;
      auto playerPosiItr = playerPositionItr (playerId);
      std::swap (playerPosiItr->playerId, std::prev (playerPosiItr, static_cast<long> (std::sqrt (myBoard.size ())) + 1)->playerId);
      return true;
    }
  else
    {
      std::cout << "did not move" << std::endl;
      return false;
    }
}

bool
Board::moveUp (std::string const &playerId)
{
  std::cout << "try moveUp" << std::endl;
  if (isAllowedToMoveUp (playerId))
    {
      std::cout << "move up" << std::endl;
      auto playerPosiItr = playerPositionItr (playerId);
      std::swap (playerPosiItr->playerId, std::prev (playerPosiItr, static_cast<long> (std::sqrt (myBoard.size ())))->playerId);
      return true;
    }
  else
    {
      std::cout << "can not move up" << std::endl;
      return false;
    }
}

bool
Board::moveUpRight (std::string const &playerId)
{
  if (isAllowedToMoveUp (playerId) && isAllowedToMoveRight (playerId))
    {
      std::cout << "try moveUpRight" << std::endl;
      auto playerPosiItr = playerPositionItr (playerId);
      std::swap (playerPosiItr->playerId, std::prev (playerPosiItr, static_cast<long> (std::sqrt (myBoard.size ())) - 1)->playerId);
      return true;
    }
  else
    {
      std::cout << "did not move" << std::endl;
      return false;
    }
}

bool
Board::moveLeft (std::string const &playerId)
{
  std::cout << "try moveLeft" << std::endl;
  if (isAllowedToMoveLeft (playerId))
    {
      std::cout << "move left" << std::endl;
      auto playerPosiItr = playerPositionItr (playerId);
      std::swap (playerPosiItr->playerId, std::prev (playerPosiItr, 1)->playerId);
      return true;
    }
  else
    {
      std::cout << "can not move left" << std::endl;
      return false;
    }
}

bool
Board::moveRight (std::string const &playerId)
{
  std::cout << "try moveRight" << std::endl;
  if (isAllowedToMoveRight (playerId))
    {
      std::cout << "move right" << std::endl;
      auto playerPosiItr = playerPositionItr (playerId);
      std::swap (playerPosiItr->playerId, std::next (playerPosiItr, 1)->playerId);
      return true;
    }
  else
    {
      std::cout << "can not move right" << std::endl;
      return false;
    }
}

bool
Board::moveDownLeft (std::string const &playerId)
{
  if (isAllowedToMoveDown (playerId) && isAllowedToMoveLeft (playerId))
    {
      std::cout << "try moveDownLeft" << std::endl;
      auto playerPosiItr = playerPositionItr (playerId);
      std::swap (playerPosiItr->playerId, std::next (playerPosiItr, static_cast<long> (std::sqrt (myBoard.size ())) - 1)->playerId);
      return true;
    }
  else
    {
      std::cout << "did not move" << std::endl;
      return false;
    }
}

bool
Board::moveDown (std::string const &playerId)
{
  std::cout << "try moveDown" << std::endl;
  if (isAllowedToMoveDown (playerId))
    {
      std::cout << "moveDown" << std::endl;
      auto playerPosiItr = playerPositionItr (playerId);
      std::swap (playerPosiItr->playerId, std::next (playerPosiItr, static_cast<long> (std::sqrt (myBoard.size ())))->playerId);
      return true;
    }
  else
    {
      std::cout << "can not moveDown" << std::endl;
      return false;
    }
}

bool
Board::moveDownRight (std::string const &playerId)
{
  if (isAllowedToMoveDown (playerId) && isAllowedToMoveRight (playerId))
    {
      std::cout << "try moveDownRight" << std::endl;
      auto playerPosiItr = playerPositionItr (playerId);
      std::swap (playerPosiItr->playerId, std::next (playerPosiItr, static_cast<long> (std::sqrt (myBoard.size ())) + 1)->playerId);
      return true;
    }
  else
    {
      std::cout << "did not move" << std::endl;
      return false;
    }
}

bool
Board::moveInDirection (std::string const &playerId, std::vector<data::Player> &players, Direction direction, boost::system::error_code const &ec)
{
  if (ec.value () == boost::system::errc::operation_canceled) return false;
  if (auto playerItr = std::find_if (players.begin (), players.end (), [&playerId] (auto const &player) { return player.playerId == playerId; }); playerItr == players.end ())
    {
      return false;
    }
  else
    {
      playerItr->moveDirection = Direction::Nothing;
    }
  switch (direction)
    {
    case Direction::UpLeft:
      {
        return moveUpLeft (playerId);
      }
    case Direction::Up:
      {
        return moveUp (playerId);
      }
    case Direction::UpRight:
      {
        return moveUpRight (playerId);
      }
    case Direction::Left:
      {
        return moveLeft (playerId);
      }
    case Direction::Nothing:
      {
        return true;
      }
    case Direction::Right:
      {
        return moveRight (playerId);
      }
    case Direction::DownLeft:
      {
        return moveDownLeft (playerId);
      }
    case Direction::Down:
      {
        return moveDown (playerId);
      }
    case Direction::DownRight:
      {
        return moveDownRight (playerId);
      }
    default:
      return false;
    }
}

void
Board::delayMoveByCostOfMovement (std::string const &playerId, std::vector<data::Player> &players, Direction direction, boost::asio::deadline_timer &playerMoveTimer)
{

  if (auto playerItr = std::find_if (players.begin (), players.end (), [&playerId] (auto const &player) { return player.playerId == playerId; }); playerItr != players.end ())
    {
      if (isAllowedTomoveInDirection (*playerItr, direction) && playerItr->moveDirection != direction)
        {
          playerItr->viewDirection = direction;
          playerItr->moveDirection = direction;
          auto const timeToMoveOneElementHorizontalOrVertical = boost::posix_time::seconds{ costToMoveOneElementHorizontalOrVertical };
          auto const timeToMoveOneElementDiagolnal = boost::posix_time::millisec{ std::lround (1000 * std::sqrt (2 * std::pow (costToMoveOneElementHorizontalOrVertical, 2))) };
          switch (direction)
            {
            case Direction::UpLeft:
            case Direction::UpRight:
            case Direction::DownLeft:
            case Direction::DownRight:
              {
                playerMoveTimer.cancel ();
                playerMoveTimer.expires_from_now (timeToMoveOneElementDiagolnal);
                playerMoveTimer.async_wait (boost::bind (&Board::moveInDirection, this, playerId, boost::ref (players), direction, boost::asio::placeholders::error));
                break;
              }
            case Direction::Up:
            case Direction::Left:
            case Direction::Right:
            case Direction::Down:
              {
                playerMoveTimer.cancel ();
                playerMoveTimer.expires_from_now (timeToMoveOneElementHorizontalOrVertical);
                playerMoveTimer.async_wait (boost::bind (&Board::moveInDirection, this, playerId, boost::ref (players), direction, boost::asio::placeholders::error));
                break;
              }
            default:
              break;
            }
        }
    }
}

size_t
Board::firstEmptyPlaceOnBoard ()
{
  if (auto boardElementWithOutPlayer = std::find_if (myBoard.begin (), myBoard.end (), [] (data::BoardElement const &boardElement) { return not boardElement.playerId.has_value (); }); boardElementWithOutPlayer != myBoard.end ())
    {
      std::cout << static_cast<size_t> (std::distance (boardElementWithOutPlayer, myBoard.begin ())) << std::endl;
      return static_cast<size_t> (std::distance (myBoard.begin (), boardElementWithOutPlayer));
    }
  else
    {
      throw std::out_of_range{ "board is full" };
      std::cout << 0 << std::endl;
      return 0;
    }
}

bool
Board::isAllowedTomoveInDirection (data::Player &player, Direction direction) const
{
  switch (direction)
    {
    case Direction::UpLeft:
      {
        return isAllowedToMoveUp (player.playerId) && isAllowedToMoveLeft (player.playerId);
      }
    case Direction::Up:
      {
        return isAllowedToMoveUp (player.playerId);
      }
    case Direction::UpRight:
      {
        return isAllowedToMoveUp (player.playerId) && isAllowedToMoveRight (player.playerId);
      }
    case Direction::Left:
      {
        return isAllowedToMoveLeft (player.playerId);
      }
    case Direction::Nothing:
      {
        return false;
      }
    case Direction::Right:
      {
        return isAllowedToMoveRight (player.playerId);
      }
    case Direction::DownLeft:
      {
        return isAllowedToMoveDown (player.playerId) && isAllowedToMoveLeft (player.playerId);
      }
    case Direction::Down:
      {
        return isAllowedToMoveDown (player.playerId);
      }
    case Direction::DownRight:
      {
        return isAllowedToMoveDown (player.playerId) && isAllowedToMoveRight (player.playerId);
      }
    default:
      return false;
    }
}
