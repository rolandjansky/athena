#ifndef ROOT_CORE_UTILS__RAN_LOAD_PACKAGES_H
#define ROOT_CORE_UTILS__RAN_LOAD_PACKAGES_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <RootCoreUtils/Global.h>

namespace RCU
{
  /// \brief whether load_packages.C has been run
  ///
  /// This is mostly meant for the EventLoop PROOF driver, which need
  /// to know whether to run this again or not.
  /// \par Guarantee
  ///   no-fail
  /// \warn this is the method that should be called to check this, as
  /// the exact mechanism for checking this may change
  bool ranLoadPackages ();
}

#endif
