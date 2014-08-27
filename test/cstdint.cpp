#include <boost/test/minimal.hpp>

#include "snl/cstdint.hpp"

DESTINATION_TYPE test_type(DESTINATION_TYPE const value)
{
    return value;
}

int test_main(int, char**)
{
    BOOST_CHECK(
	test_type(SOURCE_TYPE::max()).get_value() == SOURCE_TYPE::max().get_value());
    BOOST_CHECK(
	test_type(SOURCE_TYPE::min()).get_value() == SOURCE_TYPE::min().get_value());
    return 0;
}
