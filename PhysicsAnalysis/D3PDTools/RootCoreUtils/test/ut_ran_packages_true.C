//          Copyright Nils Krumnack 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/Global.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ranLoadPackages.h>

//
// main program
//

using namespace RCU;

void ut_ran_packages_true ()
{
  RCU_ASSERT_SOFT (ranLoadPackages() == true);
}
