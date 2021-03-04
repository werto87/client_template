#ifndef DF586E6B_A61A_4F87_A0B8_465901660A26
#define DF586E6B_A61A_4F87_A0B8_465901660A26

#include <SFML/System/Vector2.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/bind.hpp>
#include <coroutine>
#include <iostream>
#include <memory>
#include <ratio>

class DynamicValue
{
public:
  DynamicValue (boost::asio::io_context &_io_context) : timer{ _io_context } {}

  // par valueToAdd     -- total value to add to DynamicValue::value after the time is over
  // par steps          -- how many steps for example value to add is 100 steps are 5 so we add 20 every step
  // par milliseconds   -- time to add for example value to add is 100 steps are 5 and milliseconds is 1000 so we add 20 every 200 milliseconds
  boost::asio::awaitable<void>
  addToValueInStepsOverTime (sf::Vector2f _value, sf::Vector2f valueToAdd, float steps, boost::posix_time::millisec millisec)
  {
    isRunning = true;
    value = _value;
    oldValue = _value;
    for (float i = 0; i < steps; i++)
      {
        timer.expires_from_now (millisec / steps);
        try
          {
            co_await timer.async_wait (boost::asio::use_awaitable);
            value += sf::Vector2f{ valueToAdd.x / steps, valueToAdd.y / steps };
            std::cout << "value.x: " << value.x << " value.y: " << value.y << std::endl;
          }
        catch (boost::system::system_error const &ec)
          {
            std::cout << "error addToValueInStepsOverTime" << std::endl;
            std::cout << ec.what () << std::endl;
          }
      }
    std::cout << "finished addToValueInStepsOverTime" << std::endl;
  }

  void
  cancel ()
  {
    // std::cout << "cancel" << std::endl;
    if (isRunning)
      {
        timer.cancel ();
        isRunning = false;
      }
  }

  void
  reset ()
  {
    // std::cout << "reset" << std::endl;
    cancel ();
    value = oldValue;
  }

  boost::asio::deadline_timer timer;
  sf::Vector2f value{};
  sf::Vector2f oldValue{};
  bool isRunning{};
};

#endif /* DF586E6B_A61A_4F87_A0B8_465901660A26 */
