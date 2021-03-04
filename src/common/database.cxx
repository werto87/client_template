#include "src/common/database.hxx"
#include "src/common/pod.hxx"
#include <boost/mpl/for_each.hpp>
#include <confu_soci/convenienceFunctionForSoci.hxx>
void
createTables (std::string const &pathToDatabase)
{
  soci::session sql (soci::sqlite3, pathToDatabase);
  boost::mpl::for_each<tables, boost::mpl::make_identity<boost::mpl::_1> > ([&] (auto arg) {
    using T = typename decltype (arg)::type;
    try
      {
        confu_soci::createTableForStruct<T> (sql);
      }
    catch (std::exception const &e)
      {
        std::cerr << e.what () << std::endl;
      }
  });
}

void
dropTables (std::string const &pathToDatabase)
{
  soci::session sql (soci::sqlite3, pathToDatabase);
  boost::mpl::for_each<tables, boost::mpl::make_identity<boost::mpl::_1> > ([&] (auto arg) {
    using T = typename decltype (arg)::type;
    try
      {
        confu_soci::dropTableForStruct<T> (sql);
      }
    catch (std::exception const &e)
      {
        std::cerr << e.what () << std::endl;
      }
  });
}
