#include <SFML/Graphics/Color.hpp>
#include <catch2/catch.hpp>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <src/common/pod.hxx>
#include <test/constant.hxx>

TEST_CASE ("Player default construction with Direction::UpLeft")
{
  auto player = data::Player{};
  REQUIRE (player.moveDirection == Direction::UpLeft);
}

TEST_CASE ("1. create table player 2. insert player 3.find player 4. check if something was found")
{
  soci::session sql (soci::sqlite3, constant::pathToDb);
  confu_soci::createTableForStruct<data::Player> (sql);
  confu_soci::insertStruct (sql, data::Player{ .playerId = "123", .viewDirection = Direction::Up, .moveDirection = Direction::Nothing, .viewFieldsize = 0.95f, .color = sf::Color::Red });
  auto player = confu_soci::findStruct<data::Player> (sql, "playerId", "123");
  REQUIRE (player.has_value ());
}
