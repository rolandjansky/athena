#ifndef ROOT_CORE_UTILS__OVERRIDE_H
#define ROOT_CORE_UTILS__OVERRIDE_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <RootCoreUtils/Global.h>

#if __cplusplus < 201402L
#define RCU_OVERRIDE
#else
#define RCU_OVERRIDE				\
  override
#endif

#endif
