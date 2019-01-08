//          
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
#include <RootCoreUtils/StringUtil.h>

//
// main program
//

using namespace RCU;

int main ()
{
  RCU_ASSERT_SOFT (match_expr (boost::regex (".*\\.root.*"), "test.root_4"));
  RCU_ASSERT_SOFT (!match_expr (boost::regex (".*\\.root.*"), "test.asdroot_4"));
  RCU_ASSERT_SOFT (!match_expr (boost::regex (".*\\.root"), "test.root_4"));
  RCU_ASSERT_SOFT (match_expr (boost::regex (glob_to_regexp ("*.root*")), "test.root_4"));
  RCU_ASSERT_SOFT (!match_expr (boost::regex (glob_to_regexp ("*.root*")), "test.asdroot_4"));
}
