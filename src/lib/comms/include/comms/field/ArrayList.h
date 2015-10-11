//
// Copyright 2014-2015 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
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

#include <vector>

#include "comms/ErrorStatus.h"
#include "comms/options.h"
#include "comms/util/StaticQueue.h"
#include "basic/ArrayList.h"
#include "details/AdaptBasicField.h"
#include "details/OptionsParser.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TElement, typename TOptions, bool THasFixedStorage>
struct ArrayListStorageType;

template <typename TElement, typename TOptions>
struct ArrayListStorageType<TElement, TOptions, true>
{
    typedef comms::util::StaticQueue<TElement, TOptions::FixedSizeStorage> Type;
};

template <typename TElement, typename TOptions>
struct ArrayListStorageType<TElement, TOptions, false>
{
    typedef std::vector<TElement> Type;
};

template <typename TElement, typename TOptions>
using ArrayListStorageTypeT =
    typename ArrayListStorageType<TElement, TOptions, TOptions::HasFixedSizeStorage>::Type;


}  // namespace details

template <typename TFieldBase, typename TElement, typename... TOptions>
class ArrayList : public TFieldBase
{
    using Base = TFieldBase;
    typedef details::OptionsParser<TOptions...> ParsedOptionsInternal;
    using StorageTypeInternal =
        details::ArrayListStorageTypeT<TElement, ParsedOptionsInternal>;
    typedef basic::ArrayList<TFieldBase, StorageTypeInternal> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;

public:
    typedef ParsedOptionsInternal ParsedOptions;
    typedef StorageTypeInternal StorageType;
    typedef StorageType ValueType;

    /// @brief Default constructor
    /// @details Sets default value to be 0.
    ArrayList() = default;

    explicit ArrayList(const ValueType& value)
      : field_(value)
    {
    }

    explicit ArrayList(ValueType&& value)
      : field_(std::move(value))
    {
    }

    /// @brief Copy constructor is default
    ArrayList(const ArrayList&) = default;

    ArrayList(ArrayList&&) = default;

    /// @brief Destructor is default
    ~ArrayList() = default;

    /// @brief Copy assignment is default
    ArrayList& operator=(const ArrayList&) = default;

    ArrayList& operator=(ArrayList&&) = default;

    ValueType& value()
    {
        return field_.value();
    }

    const ValueType& value() const
    {
        return field_.value();
    }

    /// @brief Get length of serialised data
    constexpr std::size_t length() const
    {
        return field_.length();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        return field_.read(iter, len);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        return field_.write(iter, len);
    }

    bool valid() const
    {
        return field_.valid();
    }

    static constexpr std::size_t minLength()
    {
        return ThisField::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return ThisField::maxLength();
    }

    void forceReadElemCount(std::size_t count)
    {
        field_.forceReadElemCount(count);
    }

    void clearReadElemCount()
    {
        field_.clearReadElemCount();
    }

private:
    ThisField field_;
};

/// @brief Equivalence comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator<(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return std::lexicographical_compare(
                field1.value().begin(), field1.value().end(),
                field2.value().begin(), field2.value().end());
}

/// @brief Non-equality comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator!=(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return (field1 < field2) || (field2 < field1);
}

/// @brief Equality comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator==(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return !(field1 != field2);
}

namespace details
{

template <typename T>
struct IsArrayList
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsArrayList<comms::field::ArrayList<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isArrayList()
{
    return details::IsArrayList<T>::Value;
}


}  // namespace field

}  // namespace comms

