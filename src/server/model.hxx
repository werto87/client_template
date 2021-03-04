#ifndef C6681D77_FFD0_44D5_B5F0_95A5DD93258C
#define C6681D77_FFD0_44D5_B5F0_95A5DD93258C
#include "src/common/pod.hxx"
#include <boost/asio/deadline_timer.hpp>
#include <boost/system/system_error.hpp>
#include <src/util/util.hxx>
#include <stddef.h> // for size_t
class Board
{
public:
  Board (size_t boardSize, int costToMoveOneElementHorizontalOrVertical);

  auto playerPositionItr (std::string const &playerId);
  bool isAllowedToMoveUp (std::string const &playerId) const;
  bool isAllowedToMoveLeft (std::string const &playerId) const;
  bool isAllowedToMoveRight (std::string const &playerId) const;
  bool isAllowedToMoveDown (std::string const &playerId) const;
  bool isAllowedTomoveInDirection (data::Player &player, Direction direction) const;
  size_t playerPositionAsIndex (std::string const &playerId) const;
  bool moveUpLeft (std::string const &playerId);
  bool moveUp (std::string const &playerId);
  bool moveUpRight (std::string const &playerId);
  bool moveLeft (std::string const &playerId);
  bool moveRight (std::string const &playerId);
  bool moveDownLeft (std::string const &playerId);
  bool moveDown (std::string const &playerId);
  bool moveDownRight (std::string const &playerId);
  bool moveInDirection (std::string const &playerId, std::vector<data::Player> &players, Direction direction, boost::system::error_code const &ec);
  void delayMoveByCostOfMovement (std::string const &playerId, std::vector<data::Player> &players, Direction direction, boost::asio::deadline_timer &playerMoveTimer);
  size_t firstEmptyPlaceOnBoard ();

  std::vector<data::BoardElement> myBoard{};
  int costToMoveOneElementHorizontalOrVertical = 10;
};

#endif /* C6681D77_FFD0_44D5_B5F0_95A5DD93258C */
