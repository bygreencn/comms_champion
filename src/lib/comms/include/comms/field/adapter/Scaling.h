//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "details/AdapterBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TRatio, typename TNext>
class Scaling : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
public:

    typedef typename Base::ValueType ValueType;
    typedef TRatio ScalingRatio;

    Scaling() = default;

    explicit Scaling(const ValueType& val)
      : Base(val)
    {
    }

    explicit Scaling(ValueType&& val)
      : Base(std::move(val))
    {
    }

    Scaling(const Scaling&) = default;
    Scaling(Scaling&&) = default;
    Scaling& operator=(const Scaling&) = default;
    Scaling& operator=(Scaling&&) = default;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

