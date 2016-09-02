//          Copyright Nils Krumnack 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#undef NDEBUG
#include <RootCoreUtils/Global.h>

#include <iostream>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/PrintMsg.h>

//
// method implementations
//

int main ()
{
  RCU_PRINT_MSG ("print message");
  RCU_WARN_MSG ("warn message");
  RCU_ERROR_MSG ("error message");
  try
  {
    RCU_THROW_MSG ("throw message");
  } catch (std::exception& e)
  {
    std::cout << "caught: " << e.what() << std::endl;
  }
  RCU_ASSERT (false);
}
