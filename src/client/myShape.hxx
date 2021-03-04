#ifndef F6083EDD_E5B8_4500_A6DC_BF91FA650A4D
#define F6083EDD_E5B8_4500_A6DC_BF91FA650A4D

#include "src/util/dynamicValue.hxx"
#include <SFML/Graphics/CircleShape.hpp> // for CircleShape
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <map> // for vector
#include <memory>
#include <vector>
// predeclaration start----------------------
class Webservice;
namespace sf
{
class RenderWindow;
}
// predeclaration end----------------------

class MyShape
{
public:
  void myFunction (boost::asio::io_context &_io_context, sf::RenderWindow &window, bool leftMouseButtonReleased, bool rightMouseButtonReleased, Webservice &webservice);
  std::map<std::string, DynamicValue> playerAnimationOffset{};
  std::vector<sf::VertexArray> shapes{};
  std::vector<sf::VertexArray> players{};
};

#endif /* F6083EDD_E5B8_4500_A6DC_BF91FA650A4D */
