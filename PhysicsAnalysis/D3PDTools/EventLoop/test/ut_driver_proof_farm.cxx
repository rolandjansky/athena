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
#include <memory>
#include <TProof.h>
#include <EventLoop/ProofDriver.h>
#include <EventLoopTest/UnitTest.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/SampleLocal.h>

//
// main program
//

using namespace EL;

int main ()
{
  if (getenv ("ROOTCORE_AUTO_UT") != 0)
    return EXIT_SUCCESS;

  ProofDriver driver;
//   driver.numWorkers = 4;
  driver.returnFiles = false;
  driver.proofMaster = "acas1010";
//   driver.proofMaster = "bnlt3s01";
  driver.makeParOptions = "";
//   driver.removePar ();
  UnitTest ut ("proof_farm");//, "root://eosatlas.cern.ch//eos/atlas/user/k/krumnack/EventLoop-UnitTest/");
  ut.samples = SH::SampleHandler ();
  std::auto_ptr<SH::SampleLocal> sample (new SH::SampleLocal ("sample"));
  sample->add ("root://xrd01.usatlas.bnl.gov//data/data11_7TeV.00177986.physics_ZeroBias.merge.NTUP_JETMETEMCL.r2603_p659_p774_tid570131_00/NTUP_JETMETEMCL.570131._000001.root.1");
  sample->meta()->setString (SH::MetaFields::treeName, "qcd");
  ut.samples.add (sample.release());

  ut.cleanup = false;
  int result = ut.run (driver);
  gSystem->Exit (result);
  return result;
}
