#ifndef ASG_TOOLS__I_NAMED_INTERFACE_H
#define ASG_TOOLS__I_NAMED_INTERFACE_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#ifndef XAOD_STANDALONE

#include "GaudiKernel/INamedInterface.h"

#else

#include <string>

class INamedInterface
{
  /// virtual destructor for base class
public:
  virtual ~INamedInterface () noexcept = default;

  /// Return the name of the tool
public:
  virtual const std::string& name() const = 0;
};

#endif


#endif
