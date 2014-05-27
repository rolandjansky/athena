#ifndef ROOT_CORE_UTILS__CHECK_ROOT_VERSION_H
#define ROOT_CORE_UTILS__CHECK_ROOT_VERSION_H

//        Copyright Iowa State University 2013.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <RootCoreUtils/Global.h>

namespace RCU
{
  /// effects: check whether we are using a consistent root version
  /// guarantee: strong
  /// failures: version missmatch
  void check_root_version ();

  /// effects: disable the root version check
  /// guarantee: no-fail
  void disable_root_version_check ();
}

#endif
