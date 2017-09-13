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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TSystem.h>
#include <EventLoop/CondorDriver.h>
#include <EventLoop/DirectDriver.h>
#include <EventLoop/Job.h>
#include <EventLoop/ProofDriver.h>
#include <MultiDraw/AlgHist.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskListEOS.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SamplePtr.h>
#include <SampleHandler/ScanDir.h>
#include <SampleHandler/ToolsDiscovery.h>

//
// main program
//

int main ()
{
  if (getenv ("ROOTCORE_AUTO_UT") != 0)
    return EXIT_SUCCESS;

  SH::SampleHandler sh;
  SH::ScanDir()
    .sampleDepth (0)
    .minDepth (1)
    .maxDepth (1)
    .scan (sh, "/export/home/suyogs/data12");
  sh.setMetaString ("nc_tree", "mini");
  sh.setMetaDouble ("jobOpt", 42);
  sh.setMetaString ("mymeta", "test");

  EL::Job job;
  job.sampleHandler (sh);
  job.algsAdd (new MD::AlgHist (new TH1F ("jet_n", "jet_n", 10, 0, 10), "jet_n"));

//   EL::DirectDriver driver;
  EL::CondorDriver driver;
  TString HOME = getenv ("HOME");
  driver.shellInit = "source " + HOME + "/local-base/setup.sh || exit $? ; source " + HOME + "/local-root/setup.sh || exit $?";

  TString location = "$HOME/unit-test.$$";
  gSystem->ExpandPathName (location);

  std::cout << "storing data in " << location << std::endl;
  driver.submit (job, location.Data());
  std::cout << "success" << std::endl;
  return EXIT_SUCCESS;
}
