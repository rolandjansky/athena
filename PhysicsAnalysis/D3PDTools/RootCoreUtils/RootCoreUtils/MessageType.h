#ifndef ROOT_CORE_UTILS__MESSAGE_TYPE_H
#define ROOT_CORE_UTILS__MESSAGE_TYPE_H

//          Copyright Nils Krumnack 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines macros for reporting errors.  The interface in
/// this module is indended for experts only.  This module is
/// considered to be in the pre-alpha stage.



#include <RootCoreUtils/Global.h>

namespace RCU
{
  enum MessageType
  {
    /// description: print a regular message
    MESSAGE_REGULAR,

    /// description: print a warning
    MESSAGE_WARNING,

    /// description: print an error
    MESSAGE_ERROR,

    /// description: send out an exception
    MESSAGE_EXCEPTION,

    /// description: print and abort
    MESSAGE_ABORT,

    /// description: unspecified message type
    MESSAGE_UNSPECIFIED
  };
}

#endif
