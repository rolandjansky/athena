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

#include <EventLoop/ProofDriver.h>
#include <EventLoop/Job.h>
#include <EventLoopTest/UnitTest.h>

//
// main program
//

using namespace EL;

int main ()
{
  return 0;

  /*
  ProofDriver driver;
  // driver.makeParOptions = "";
  // driver.removePar ();
  driver.numWorkers = 4;
  driver.options()->setBool (Job::optBackgroundProcess, true);
  driver.proofParm.setDouble ("PROOF_UseTreeCache", 0);
  driver.proofParm.setString ("PROOF_LookupOpt", "none");
  UnitTest ut ("proof_lite_bg");
  ut.testOutput = false;
  ut.testFileExecute = false;
  // ut.cleanup = false;
  // ut.location = "$HOME/unit-test.$$";
  return ut.run (driver);
  */
}
