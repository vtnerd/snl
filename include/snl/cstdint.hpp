#ifndef SNL_CSTDINT_HPP
#define SNL_CSTDINT_HPP

#include <cstdint>
#include <limits>
#include <type_traits>

namespace snl
{
    template<typename> class check_conversion;

    template<typename>
    struct is_check_conversion : std::false_type {};

    template<typename ScalarType>
    struct is_check_conversion<check_conversion<ScalarType>> : std::true_type {};
}

namespace std
{
    template<typename ScalarType1, typename ScalarType2>
    struct common_type<check_conversion<ScalarType1>, check_conversion<ScalarType2>>
    {
	typedef check_conversion<common_type_t<ScalarType1, ScalarType2>> type;
    };

    template<typename ScalarType, typename OtherType>
    struct common_type<check_conversion<ScalarType>, OtherType>
    {
	typedef check_conversion<common_type_t<ScalarType, OtherType>> type;
    };

    template<typename OtherType, typename ScalarType>
    struct common_type<OtherType, check_conversion<ScalarType>>
    {
	typedef check_conversion<common_type_t<OtherType, ScalarType>> type;
    };
}

namespace snl
{
    template<typename ScalarType>
    class check_conversion
    {
    private: // Type contraints for class

        /*!
          \note This class copies all templated parameters, and all types of
              check_conversion because only scalar values are allowed. Instances
              of this template are designed to be treated exactly like scalars.
         */
        static_assert(
            std::is_scalar<ScalarType>::value,
            "Currently only built-in scalar types are supported");

    public: // Static traits for template type

        //! \return True if the current ScalarType is signed.
        static constexpr bool is_signed()
        {
            return std::numeric_limits<ScalarType>::is_signed;
        }

        //! \return The minimum value for the curent ScalarType.
        static constexpr ScalarType min()
        {
            return std::numeric_limits<ScalarType>::min();
        }

        //! \return The maximum value for the current ScalarType.
        static constexpr ScalarType max()
        {
            return std::numeric_limits<ScalarType>::max();
        }

        /*!
          Determines whether an implicit conversion from SourceType could result
          in data loss.

          \tparam SourceType Another check_conversion type.

          \return True if ScalarType can store all possible values from
              SourceType::ScalarType
         */
        template<typename SourceType>
        static constexpr bool allow_implicit_conversion()
        {
            // if source is signed but destination is not, abort
            if (SourceType::is_signed() && !is_signed())
            {
                return false;
            }

            // is source is unsigned, then destination can always
            // store min value
            if (!SourceType::is_signed())
            {
                // if destination is signed, it will implicitly convert to
                // unsigned in this expression, which is fine
                return SourceType::max() <= max();
            }

            // only remaining case is signed -> signed
            return SourceType::min() >= min() && SourceType::max() <= max();
        }

    private: // Computation for conversions. MUST be after the static
             // template traits, the template keyword does not delay
             // the lookup for some reason.


        template<typename SourceType>
        struct enable_implicit_conversion_helper :
            std::integral_constant<
                bool,
                check_conversion<ScalarType>::template
                    allow_implicit_conversion<check_conversion<SourceType>>()>
        {
        };

        template<typename SourceType>
        struct enable_implicit_conversion_helper<check_conversion<SourceType>> :
            std::integral_constant<
                bool,
                check_conversion<ScalarType>::template
                    allow_implicit_conversion<check_conversion<SourceType>>()>
        {
        };

        template<typename SourceType>
        using enable_implicit_conversion =
            typename std::enable_if<
                enable_implicit_conversion_helper<SourceType>::value,
                void*>::type;

        template<typename SourceType>
        using enable_explicit_conversion =
            typename std::enable_if<
                !enable_implicit_conversion_helper<SourceType>::value,
                void*>::type;

        template<typename SourceScalarType>
        static constexpr SourceScalarType extract_value(
            SourceScalarType const value)
        {
            return value;
        }

        template<typename SourceScalarType>
        static constexpr SourceScalarType extract_value(
            check_conversion<SourceScalarType> const value)
        {
            return value.get_value();
        }

    public: // Constructors and functions

        /*!
          Convert from a scalar value. Disabled if this destination type cannot
          store all values from the SourceScalarType.

          \tparam ScalarType Only types meeting is_scalar<> type trait.

          \param[in] value The value to be represented by the checked class.
         */
        template<typename SourceType>
        check_conversion(
                SourceType const value,
                enable_implicit_conversion<SourceType> = 0) :
            value_(extract_value(value))
        {
        }

        template<typename SourceType>
        explicit check_conversion(
                SourceType const value,
                enable_explicit_conversion<SourceType> = 0) :
            value_(extract_value(value))
        {
        }

        void add(check_conversion const add_value)
        {
            return value_ += add_value.get_value();
        }

        void subtract(check_conversion const subtract_value)
        {
            return value_ -= subtract_value.get_value();
        }

        void multiply(check_conversion const multiply_value)
        {
            return value_ *= multiply_value.get_value();
        }

        void divide(check_conversion const divide_value)
        {
            return value_ /= divide_value.get_value();
        }

        // \return The current scalar value being wrapped.
        ScalarType get_value() const
        {
            return value_;
        }

    private:

        ScalarType value_;
    };

    typedef check_conversion<std::int8_t>     int8_t;
    typedef check_conversion<std::uint8_t>   uint8_t;
    typedef check_conversion<std::int16_t>   int16_t;
    typedef check_conversion<std::uint16_t> uint16_t;
    typedef check_conversion<std::int32_t>   int32_t;
    typedef check_conversion<std::uint32_t> uint32_t;
    typedef check_conversion<std::int64_t>   int64_t;
    typedef check_conversion<std::uint64_t> uint64_t;


    //
    // OPERATORS
    //
    namespace checked_detail
    {
	template<typename Type1, typename Type2>
	inline constexpr bool allow_common_conversion()
	{
	    typedef common_type_t<Type1 Type2> CommonType;
	    return CommonType<Type1>::allow_implicit_conversion() &&
		CommonType<Type2>::allow_implicit_conversion();
	}

	template<typename Type1, typename Type2>
	using enable_operator =
	    std::enable_if<
	        allow_common_conversion<Type1, Type2>(),
	        common_type_t<Type1, Type2>>::type;
    }

    template<typename ScalarType, typename OtherType>
    checked_detail::enable_operator<check_conversion<ScalarType>, OtherType>
    operator+(
	check_conversion<ScalarType> const lhs, OtherType const rhs)
    {
	common_type_t<check_conversion<ScalarType>, OtherType> lhs_copy = lhs;
	lhs_copy.add(rhs);
	return lhs_copy;
    }

    template<typename OtherType, typename ScalarType>
    checked_detail::enable_operator<OtherType, check_conversion<ScalarType>>
    operator+(
	 OtherType const lhs, check_conversion<ScalarType> const rhs)
    {
	common_type_t<check_conversion<ScalarType>, OtherType> lhs_copy = lhs;
	lhs_copy.add(rhs);
	return lhs_copy;
    }
}

#endif // SNL_CSTDINT_HPP
