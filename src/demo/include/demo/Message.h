//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

/// @file
/// @brief Contains common interface for all the messages of demo binary protocol.

#pragma once

#include <cstdint>

#include "comms/comms.h"

#include "MsgId.h"

namespace comms_champion
{

namespace demo
{

/// @brief Interface class of all the Demo binary protocol messages.
/// @details The class publicly inherits from
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1Message.html" target="_black">comms::Message</a>
///     class while providing the following default options:
///     @li @b comms::option::MsgIdType<MsgId> > - use @ref MsgId as the type of message ID.
///     @li @b comms::option::BigEndian - use big endian for serialisation.
///     @li @b comms::option::RefreshInterface - add refresh() member function to interface.
///
///     All other options provided with TOptions template parameter will also be passed
///     to the @b comms::Message base class to define the interface.
/// @tparam TOptions Zero or more extra options to be passed to the @b comms::Message
///     base class to define the interface.
template <typename... TOptions>
class MessageT : public
    comms::Message<
        TOptions...,
        comms::option::BigEndian,
        comms::option::MsgIdType<MsgId> >,
        comms::option::RefreshInterface
{
public:

    /// @brief Default constructor
    MessageT() = default;

    /// @brief Copy constructor
    MessageT(const MessageT&) = default;

    /// @brief Move constructor
    MessageT(MessageT&&) = default;

    /// @brief Destructor
    virtual ~MessageT() = default;

    /// @brief Copy assignment operator
    MessageT& operator=(const MessageT&) = default;

    /// @brief Move assignment operator
    MessageT& operator=(MessageT&&) = default;

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Refresh the contents of the message.
    /// @details In Demo binary protocol in some messages contents of specific
    ///     fields may limit the value of other field(s). A call to this function
    ///     is intended to bring the contents of the message to a valid state
    ///     after one or more of the fields was updated. @n
    ///     This function calls refreshImpl() virtual function and returns the value
    ///     returned by the latter. The expected behaviour from overriding functions
    ///     in the derived classes is to return @b true if value of at least one
    ///     field was modified, and @b false if values of all the fields remained intact.
    /// @return Whatever call to virtual refreshImpl() returns.
    bool refresh();
#endif // #ifdef FOR_DOXYGEN_DOC_ONLY

protected:

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Polymorphic refresh functionality.
    /// @details Invoked by non-virtual refresh() interface function. By default
    ///     does nothing and returns false. It may be overridden by the
    ///     derived class.
    /// @return Default implementation always returns @b false.
    virtual bool refreshImpl();
#endif // #ifdef FOR_DOXYGEN_DOC_ONLY
};

/// @brief Default Demo interface class.
/// @details It is a typedef of MessageT class with extra following options:
///     @li <b> comms::option::ReadIterator<const std::uint8_t*></b> -
///         Enables read operation and specifies <b>const std::uint8_t*</b> as
///         the iterator used for reading.
///     @li <b> comms::option::WriteIterator<std::uint8_t*></b> -
///         Enables write operation and specifies <b>std::uint8_t*</b> as
///         the iterator used for writing.
typedef MessageT<
    comms::option::ReadIterator<const std::uint8_t*>,
    comms::option::WriteIterator<std::uint8_t*>
> Message;

}  // namespace demo

} // namespace comms_champion


