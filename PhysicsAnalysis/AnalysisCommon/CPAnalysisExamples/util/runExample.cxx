/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE

// Adapted from SoftwareTutorialxAODAnalysisInROOT
// argv[1] = clistFile
// argv[2] = jobName (relative to .)
// argv[3] = proofFlag (none/lite/farm)
// argv[4] = evtMax

// Stearing code for the xExample code

#include "CPAnalysisExamples/xExample.h"
#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoop/ProofDriver.h"

#include <TFile.h>
#include <TChain.h>
#include <TSystem.h>

#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

int main( int argc, char* argv[] ) {

  // Check arguments
  if( argc < 4 ){
    std::cerr <<"Usage: clistFile jobName proofFlag maxEvt" <<std::endl;
    std::cerr <<"proofFlag = none, lite, or farm name" <<std::endl;
    std::cerr <<"Use maxEvt = 0 for all events" <<std::endl;
    return 1;
  }
  std::string clistFile = argv[1];
  std::string jobName = argv[2];
  std::string proofFlag = argv[3];
  int evtMax = 0;
  evtMax = std::atoi(argv[4]);

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Open the input .clist file and create TChain
  Info("runExample", "Opening .clist file: %s", clistFile.c_str() );
  std::ifstream inFile( clistFile.c_str() );
  TChain chain("CollectionTree");
  std::string nextFile;
  while( inFile >> nextFile ){
    Info("runExample", "Adding to TChain file: %s", nextFile.c_str() );
    chain.Add( nextFile.c_str() );
  }

  // Create a new sample handler for the TChain
  SH::SampleHandler sh;
  sh.add( SH::makeFromTChain(jobName.c_str(),chain) );
  sh.setMetaString( "nc_tree", "CollectionTree" );

  // Create EventLoop job
  // Driver writes output in subdirectory jobName
  // Atilla: need class mode for electrons
  EL::Job job;
  job.useXAOD();
  job.options()->setString (EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_class);
  job.sampleHandler(sh);

  xExample* xt = new xExample;
  job.algsAdd( xt );
  if( evtMax > 0 ) job.options()->setDouble(EL::Job::optMaxEvents, evtMax);

  // From https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EventLoop#PROOF_Driver
  if( proofFlag == "none" ){
    std::cout <<"runExample running locally" <<std::endl;
    EL::DirectDriver driver;
    driver.submit( job, jobName.c_str() );
  } else if( proofFlag == "lite" ){
    std::cout <<"runExample running with ProofLite" <<std::endl;
    EL::ProofDriver driver;
    driver.submit( job, jobName.c_str() );
    gSystem->Exit (0);
  } else {
    std::cout <<"runExample running with Proof on " <<proofFlag <<std::endl;
    EL::ProofDriver driver;
    driver.proofMaster = proofFlag.c_str();
    driver.makeParOptions = "";
    driver.returnFiles = false;
    driver.submit( job, jobName.c_str() );
    gSystem->Exit (0);
  }

  return 0;
}

#endif