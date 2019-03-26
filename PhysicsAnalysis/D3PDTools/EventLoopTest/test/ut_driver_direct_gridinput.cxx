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

#include <EventLoop/DirectDriver.h>
#include <EventLoopTest/UnitTest.h>

//
// main program
//

using namespace EL;

int main ()
{
  DirectDriver driver;
  UnitTest ut ("direct_gridinput");
  ut.gridInput = true;
  // ut.cleanup = false;
  // ut.location = "$HOME/unit-test.$$";
  return ut.run (driver);
}
