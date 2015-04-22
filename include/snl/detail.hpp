#ifndef SNL_DETAIL_HPP
#define SNL_DETAIL_HPP

#if 201103L <= __cplusplus
  #define SNL_CPP11
  #include <limits>
  #include <type_traits>
#else // Legacy mode
  #include <boost/integer_traits.hpp>
  #include <boost/utility/enable_if.hpp>
#endif

namespace snl
{
    template<typename> class check_conversion;

    namespace detail
    {
#ifdef SNL_CPP11
        template<bool Test>
        struct enable_if : std::enable_if<Test, void*>
        {
        };

        template<typename Type>
        struct min
        {
            const static Type value = std::numeric_limits<Type>::min();
        };

        template<typename Type>
        struct max
        {
            const static Type value = std::numeric_limits<Type>::max();
        };
#else // Legacy mode
        template<bool Test>
        struct enable_if : boost::enable_if_c<Test, void*>
        {
        };

        template<typename Type>
        struct min
        {
            const static Type value = boost::integer_traits<Type>::const_min;
        };

        template<typename Type>
        struct max
        {
            const static Type value = boost::integer_traits<Type>::const_max;
        };
#endif

        enum conversion_situation
        {
            kSourceUnsignedDestinationUnsigned = 0,
            kSourceSignedDestinationUnsigned,
            kSourceUnsignedDestinationSigned,
            kSourceSignedDestinationSigned,
        };

        template<typename Source, typename Destination>
        struct determine_situation
        {
            const static unsigned source_signed =
                std::numeric_limits<Source>::is_signed;
            const static unsigned destination_signed =
                std::numeric_limits<Destination>::is_signed;

            const static conversion_situation value =
                static_cast<conversion_situation>(source_signed + destination_signed * 2);
        };

        template<typename Source, typename Destination, conversion_situation>
        struct compute_narrowing
        {
            // Source is unsigned, so only check destination max
            const static bool value = max<Destination>::value < max<Source>::value;
        };

        template<typename Source, typename Destination>
        struct compute_narrowing<Source, Destination, kSourceSignedDestinationSigned>
        {
            const static bool value =
                min<Source>::value < min<Destination>::value ||
                max<Destination>::value < max<Source>::value;
        };

        template<typename Source, typename Destination>
        struct compute_narrowing<Source, Destination, kSourceSignedDestinationUnsigned>
        {
            const static bool value = true;
        };

        template<typename Source, typename Destination>
        struct is_narrowing :
            compute_narrowing<Source, Destination, determine_situation<Source, Destination>::value>
        {
        };
    }
}

#endif // SNL_DETAIL_HPP
