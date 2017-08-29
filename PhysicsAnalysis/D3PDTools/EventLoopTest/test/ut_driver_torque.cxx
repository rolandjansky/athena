/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/Global.h>

#include <cstdlib>
#include <EventLoop/TorqueDriver.h>
#include <EventLoopTest/UnitTest.h>

//
// main program
//

using namespace EL;

int main ()
{
  if (getenv ("ROOTCORE_AUTO_UT") != 0)
    return EXIT_SUCCESS;

  const std::string HOME = getenv ("HOME");

  TorqueDriver driver;
  driver.shellInit = "source " + HOME + "/local-base/setup.sh || exit $? ; source " + HOME + "/local-root/setup.sh || exit $?";
  UnitTest ut ("torque");
  ut.cleanup = false;
  ut.location = "$HOME/unit-test.$$";
  return ut.run (driver);
}
