# SNL (Safe Numerics Library) #
A C++ Safe Numerics Library _(in-progress)_. Designed to replace the built-in C++ scalar types with safer types. 

## See Robert Ramey's Library Instead ##
I started this library before I knew about [Robert's version](https://github.com/robertramey/safe_numerics) in the boost incubator. I then considered making a radical change - using `boost::expected` to report failures instead of exceptions. I am not sure on the direction of this change, because I think it will require more "boilerplate" to use the non-exception way than most are willing to accept. Progress on this library may continue in the future if I change my mind.

### check_conversion ###
##### Implicit Conversions #####
Standard types (`snl::uint16_t`, etc.) have the implicit conversion disabled when destination type cannot safely store all values from the source type. This provides stricter checking between interfaces; a function signature change could fail to compile, providing better analysis at compilation time.

Example:
```c++
#include <boost/test/minimal.hpp>

#include "snl/cstdint.hpp"

snl::uint8_t foo(snl::uint8_t const value)
{
    return value;
}

int test_main(int, char**)
{
    BOOST_CHECK(foo(std::uint8_t(0)).get_value() == 0);
    BOOST_CHECK(foo(snl::uint8_t(0)).get_value() == 0);
    BOOST_CHECK(foo(std::uint16_t(0)).get_value() == 0);
    BOOST_CHECK(foo(snl::uint16_t(0)).get_value() == 0);
    return 0;
}
```
This file will produce these error messages:
```
implicit_conversion.cpp:5:13: note: candidate function not viable: no known conversion from 'std::uint16_t' (aka 'unsigned short') to 'const snl::uint8_t' (aka 'const check_conversion<std::uint8_t>') for 1st argument
implicit_conversion.cpp:5:13: note: candidate function not viable: no known conversion from 'check_conversion<std::uint16_t>' to 'const check_conversion<std::uint8_t>' for 1st argument
```
The first two calls to `foo` are not an error because `uint8_t` can store all values from an `uint8_t`. The second two produces conversion errors because `uint8_t` cannot store all values from a `uint16_t`. This intelligent error detecting is not limited to function signatures; any mathematical or comparison operation that requires an implicit conversion will be checked. This will force the programmer to add more explicit casting, but this also prevents code changes from producing incorrect conversions that should be caught at compile time.

##### Comparison Operators #####
The comparison operators between signed/unsigned will automatically handle arthmetic in the manner _expected_ by most programmers. A signed value will not automatically convert to an unsigned value in a comparison operation, unless the signed value is positive. If the signed value is negative, it will return the "correct" result:
```c++
   const std::int64_t standard1 = -1;
   const std::uint64_t standard2 = std::numeric_limits<std::uint64_t>::max();
   const snl::int64_t snl1 = standard1;
   const snl::uint64_t snl2 = standard2;
   
   EXPECT_FALSE(standard1 < standard); // undefined behavior (signed underflow)
   EXPECT_TRUE(snl1 < snl2);
```

## check_overflow (C++11 only) ##
Checked types (`snl::checked_uint16_t`, etc.) will check for overflow before arthimetic operations, and return an error code in a `boost::expected` object on failure.
