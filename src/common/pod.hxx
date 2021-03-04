#ifndef E492CD2F_D464_4461_985D_81352DBBC07D
#define E492CD2F_D464_4461_985D_81352DBBC07D
#include <SFML/Graphics/Color.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/algorithm/query/count.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/count.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/type_index.hpp>
#include <cstdint>
#include <magic_enum.hpp>
#include <soci/exchange-traits.h>
#include <soci/soci.h>
#include <soci/use-type.h>
#include <soci/values-exchange.h>

//########################### Enum ################################
enum struct BoardElementType : char
{
  Plain,
  Hill,
  Valley
};
enum class Direction : char
{
  UpLeft,
  Up,
  UpRight,
  Left,
  Nothing,
  Right,
  DownLeft,
  Down,
  DownRight
};
//#################################################################
//######################### Define Struct #########################
namespace data
{
struct BoardElement
{
  unsigned long id{};
  sf::Color color = sf::Color::Blue;
  boost::optional<std::string> playerId{};
  BoardElementType boardElementType{};
  float movementCostToMoveThroughVerticalOrHorizontal = 1.f;
};
}
BOOST_FUSION_ADAPT_STRUCT (data::BoardElement, (unsigned long, id) (sf::Color, color) (boost::optional<std::string>, playerId) (BoardElementType, boardElementType) (float, movementCostToMoveThroughVerticalOrHorizontal))
BOOST_FUSION_DEFINE_STRUCT ((data), Player, (std::string, playerId) (Direction, viewDirection) (Direction, moveDirection) (double, viewFieldsize) (sf::Color, color))
//#################################################################
//####################### Table to Generate #######################
typedef boost::mpl::list<data::Player, data::BoardElement> tables;
//#################################################################
//######################### Serialization #########################
namespace boost::serialization
{
template <class Archive>
void
serialize (Archive &ar, data::BoardElement &boardElement, const unsigned int)
{
  ar &boardElement.color;
  ar &boardElement.playerId;
  ar &boardElement.boardElementType;
  ar &boardElement.movementCostToMoveThroughVerticalOrHorizontal;
}

template <class Archive>
void
serialize (Archive &ar, sf::Color &color, const unsigned int)
{
  ar &color.r;
  ar &color.g;
  ar &color.b;
  ar &color.a;
}

template <class Archive>
void
serialize (Archive &ar, data::Player &player, const unsigned int)
{
  ar &player.playerId;
  ar &player.viewDirection;
  ar &player.viewFieldsize;
  ar &player.color;
  ar &player.moveDirection;
}
}
//#################################################################
//##################### Database Conversation #####################
namespace soci
{
template <> struct type_conversion<sf::Color>
{
  typedef sf::Uint32 base_type;

  static void
  from_base (base_type const &v, indicator /* ind */, sf::Color &p)
  {
    p = sf::Color{ v };
  }

  static void
  to_base (const sf::Color &p, base_type &v, indicator &ind)
  {
    v = p.toInteger ();
    ind = i_ok;
  }
};
}
//#################################################################
#endif /* E492CD2F_D464_4461_985D_81352DBBC07D */
