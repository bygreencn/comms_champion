//
// Copyright 2014 - 2015 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <limits>
#include "comms/Field.h"

#include "comms/util/SizeToType.h"
#include "details/AdaptBasicField.h"
#include "details/OptionsParser.h"
#include "IntValue.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TOptionsBundle, bool THasFixedLength>
struct BitmaskUndertlyingType;

template <typename TOptionsBundle>
struct BitmaskUndertlyingType<TOptionsBundle, true>
{
    typedef typename comms::util::SizeToType<TOptionsBundle::FixedLength, false>::Type Type;
};

template <typename TOptionsBundle>
struct BitmaskUndertlyingType<TOptionsBundle, false>
{
    static_assert(!TOptionsBundle::HasVarLengthLimits,
        "Bitmask with variable length is not supported.");
    typedef unsigned Type;
};

template <typename TOptionsBundle>
using BitmaskUndertlyingTypeT =
    typename BitmaskUndertlyingType<TOptionsBundle, TOptionsBundle::HasFixedLengthLimit>::Type;


}  // namespace details

/// @brief Bitmask value field.
/// @details Quite often communication protocols specify bitmask values, where
///     any bit has a specific meaning. Although such masks are can be handled
///     as unsigned integer values using comms::field::IntValue field type,
///     using comms::field::Bitmask may be a bit more convenient.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field. If no option is provided, the underlying type is assumed
///     to be "unsigned", which is usually 4 bytes long. To redefined the length
///     of the bitmask field, use comms::option::FixedLength option.
///     For example:
///     @code
///         using MyFieldBase = comms::Field<comms::option::BigEndian>;
///         using MyField =comms::field::EnumValue<MyFieldBase>;
///     @endcode
///     The serialised value of the field in the example above will consume
///     sizeof(unsigned) bytes, because the underlying type chosen to be "unsigned"
///     by default. Example below specifies simple bitmask value field with
///     2 bytes serialisation length:
///     @code
///         using MyFieldBase = comms::Field<comms::option::BigEndian>;
///         using MyField =comms::field::EnumValue<MyFieldBase, comms::option::FixedLength<2> >;
///     @endcode
///     Supported options are:
///     @li comms::option::FixedLength
///     @li comms::option::FixedBitLength
///     @li comms::option::DefaultValueInitialiser or comms::option::DefaultNumValue.
///     @li comms::option::ContentsValidator or comms::option::BitmaskReservedBits.
///     @li comms::option::FailOnInvalid
///     @li comms::option::IgnoreInvalid
template <typename TFieldBase, typename... TOptions>
class BitmaskValue : public TFieldBase
{
    typedef TFieldBase Base;

    typedef details::OptionsParser<TOptions...> OptionsBundle;

    typedef details::BitmaskUndertlyingTypeT<OptionsBundle> IntValueType;

    typedef
        IntValue<
            TFieldBase,
            IntValueType,
            TOptions...
        > IntValueField;

public:

    /// @brief All the options provided to this class bundled into struct.
    typedef OptionsBundle ParsedOptions;

    /// @brief Type of underlying integral value.
    /// @details Unsigned integral type, which depends on the length of the
    ///     mask determined by the comms::option::FixedLength option.
    typedef typename IntValueField::ValueType ValueType;

    /// @brief Default constructor.
    /// @brief Initial bitmask has all bits cleared (equals 0)
    BitmaskValue() = default;

    /// @brief Constructor
    explicit BitmaskValue(const ValueType& val)
       : intValue_(val)
    {
    }

    /// @brief Copy constructor
    BitmaskValue(const BitmaskValue&) = default;

    /// @brief Destructor
    ~BitmaskValue() = default;

    /// @brief Copy assignment
    BitmaskValue& operator=(const BitmaskValue&) = default;

    /// @brief Get access to underlying mask value storage.
    const ValueType& value() const
    {
        return intValue_.value();
    }

    /// @brief Get access to underlying mask value storage.
    ValueType& value()
    {
        return intValue_.value();
    }

    /// @brief Get length required to serialise the current field value.
    constexpr std::size_t length() const
    {
        return intValue_.length();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    static constexpr std::size_t maxLength()
    {
        return IntValueField::maxLength();
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    static constexpr std::size_t minLength()
    {
        return IntValueField::minLength();
    }

    /// @brief Read field value from input data sequence
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] size Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return intValue_.read(iter, size);
    }

    /// @brief Write current field value to output data sequence
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return intValue_.write(iter, size);
    }

    /// @brief Check validity of the field value.
    constexpr bool valid() const
    {
        return intValue_.valid();
    }

    /// @brief Check whether all bits from provided mask are set.
    /// @param[in] mask Mask to check against
    /// @return true in case all the bits are set, false otherwise
    bool hasAllBitsSet(ValueType mask) const
    {
        return (value() & mask) == mask;
    }

    /// @brief Check whether any bits from provided mask are set.
    /// @param[in] mask Mask to check against
    /// @return true in case at least one of the bits is set, false otherwise.
    bool hasAnyBitsSet(ValueType mask) const
    {
        return (value() & mask) != 0;
    }

    /// @brief Set all the provided bits.
    /// @details Equivalent to @code value() |= mask; @endcode
    /// @param[in] mask Mask of bits to set.
    void setBits(ValueType mask)
    {
        value() |= mask;
    }

    /// @brief Set all the provided bits.
    /// @details Equivalent to @code value() &= (~mask); @endcode
    /// @param[in] mask Mask of bits to clear.
    void clearBits(ValueType mask)
    {
        value() &= (~mask);
    }

    /// @brief Get bit value
    bool getBitValue(unsigned bitNum) const
    {
        return hasAllBitsSet(
            static_cast<ValueType>(1U) << bitNum);
    }

    /// @brief Set bit value
    void setBitValue(unsigned bitNum, bool val)
    {
        auto mask = static_cast<ValueType>(1U) << bitNum;
        if (val) {
            setBits(mask);
        }
        else {
            clearBits(mask);
        }
    }

private:

    IntValueField intValue_;
};

// Implementation

/// @brief Equality comparison operator.
/// @related BitmaskValue
template <typename... TArgs>
bool operator==(
    const BitmaskValue<TArgs...>& field1,
    const BitmaskValue<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @related BitmaskValue
template <typename... TArgs>
bool operator!=(
    const BitmaskValue<TArgs...>& field1,
    const BitmaskValue<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @related BitmaskValue
template <typename... TArgs>
bool operator<(
    const BitmaskValue<TArgs...>& field1,
    const BitmaskValue<TArgs...>& field2)
{
    return field1.value() < field2.value();
}

namespace details
{

template <typename T>
struct IsBitmaskValue
{
    static const bool Value = false;
};

template <typename TFieldBase, typename... TOptions>
struct IsBitmaskValue<comms::field::BitmaskValue<TFieldBase, TOptions...> >
{
    static const bool Value = true;
};

}  // namespace details

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::BitmaskValue.
/// @tparam T Any type.
/// @related comms::field::BitmaskValue
template <typename T>
constexpr bool isBitmaskValue()
{
    return details::IsBitmaskValue<T>::Value;
}

}  // namespace field

}  // namespace comms


