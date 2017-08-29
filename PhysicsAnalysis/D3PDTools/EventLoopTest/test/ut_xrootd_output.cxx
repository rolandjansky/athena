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
#include <EventLoop/DirectDriver.h>
#include <EventLoopTest/UnitTest.h>
#include <RootCoreUtils/ShellExec.h>
#include <SampleHandler/DiskOutputXRD.h>

//
// main program
//

using namespace EL;

int main ()
{
  if (getenv ("ROOTCORE_AUTO_UT") != 0)
    return EXIT_SUCCESS;

  DirectDriver driver;
  UnitTest ut ("direct");
  ut.outputDisk = new SH::DiskOutputXRD ("root://hep-head1.physics.iastate.edu:1094//local/xrootd/a/nils/test1/");
  ut.cleanup = false;
  // ut.location = "$HOME/unit-test.$$";
  RCU::Shell::exec ("rm -rf /mnt/nils/test1");
  return ut.run (driver);
}
