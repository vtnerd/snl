SNL (Safe Numerics Library)
===

A C++14 Safe Numerics Library _(in-progress)_. Designed to replace the built-in C++ scalar types with safer types. 

check_conversion
---
Standard types (snl::uint16_t, etc.) have implicit conversion disabled when destination type cannot safely store all values from the source type. This provides stricter checking between interfaces; a function signature change will fail to compile, providing better analysis at compilation time.

check_overflow
---
Checked types (snl::checked_uint16_t, etc.) will check for overflow before arthimetic operations, and throw exceptions on failure. Named member functions also allow for overflow checking, without the use of exceptions (return code).
