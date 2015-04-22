#ifndef SNL_CSTDINT_HPP
#define SNL_CSTDINT_HPP

#include "detail.hpp"

#ifdef SNL_CPP11
  #include <cstdint>
#else // Legacy mode
  #include <boost/cstdint.hpp>
  #include <boost/utility/enable_if.hpp>
#endif

namespace snl
{
#ifdef SNL_CPP11
    typedef check_conversion<std::int8_t>     int8_t;
    typedef check_conversion<std::uint8_t>   uint8_t;
    typedef check_conversion<std::int16_t>   int16_t;
    typedef check_conversion<std::uint16_t> uint16_t;
    typedef check_conversion<std::int32_t>   int32_t;
    typedef check_conversion<std::uint32_t> uint32_t;
    typedef check_conversion<std::int64_t>   int64_t;
    typedef check_conversion<std::uint64_t> uint64_t;
#else // Legacy mode
    typedef check_conversion<boost::int8_t>     int8_t;
    typedef check_conversion<boost::uint8_t>   uint8_t;
    typedef check_conversion<boost::int16_t>   int16_t;
    typedef check_conversion<boost::uint16_t> uint16_t;
    typedef check_conversion<boost::int32_t>   int32_t;
    typedef check_conversion<boost::uint32_t> uint32_t;
    typedef check_conversion<boost::int64_t>   int64_t;
    typedef check_conversion<boost::uint64_t> uint64_t;
#endif

    template<typename Type>
    class check_conversion
    {
    private:
        template<typename Source, typename Destination>
        struct invalid_conversion : detail::is_narrowing<Source, Destination>
        {
        };

        template<typename Source, typename Destination>
        struct invalid_conversion<check_conversion<Source>, Destination> :
            detail::is_narrowing<Source, Destination>
        {
        };

        template<typename SourceType>
        static SourceType extract_value(
            SourceType const value)
        {
            return value;
        }

        template<typename SourceType>
        static SourceType extract_value(
            check_conversion<SourceType> const value)
        {
            return value.get_value();
        }

    public: // Constructors and functions
        /*!
          Convert from a scalar value. Disabled if this destination type cannot
          store all values from the SourceType.

          \tparam Type Only types meeting is_scalar<> type trait.

          \param[in] value The value to be represented by the checked class.
         */
        template<typename SourceType>
        check_conversion(
            SourceType const value,
            typename detail::enable_if<!invalid_conversion<SourceType, Type>::value>::type = 0)
            : value_(extract_value(value))
        {
        }

        template<typename SourceType>
        explicit check_conversion(
            SourceType const value,
            typename detail::enable_if<invalid_conversion<SourceType, Type>::value>::type = 0)
            : value_(extract_value(value))
        {
        }

        check_conversion& operator=(check_conversion const rhs) {
            value_ = rhs.get_value();
            return *this;
        }

        // \return The current scalar value being wrapped.
        Type get_value() const
        {
            return value_;
        }

    private:
        Type value_;
    };
}

#endif // SNL_CSTDINT_HPP
