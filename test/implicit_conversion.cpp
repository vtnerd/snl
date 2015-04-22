#include <boost/test/minimal.hpp>

#include "snl/cstdint.hpp"

#ifdef SNL_CPP11
using namespace std;
#else
using namespace boost;
#endif

DESTINATION_TYPE test_implicit_conversion(
    DESTINATION_TYPE const value)
{
    return value;
}

int test_main(int, char**)
{
    BOOST_CHECK(test_implicit_conversion(SOURCE_TYPE(0)).get_value() == 0);
    return 0;
}
