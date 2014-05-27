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

#include <iostream>
#include <RootCoreUtils/Locate.h>

//
// method implementations
//

int main (int argc, char **argv)
{
  if (argc != 2)
  {
    std::cerr << "usage: " << argv[0] << " locations" << std::endl;
    return 1;
  }

  std::cout << RCU::locate (argv[1]) << std::endl;
  return 0;
}
