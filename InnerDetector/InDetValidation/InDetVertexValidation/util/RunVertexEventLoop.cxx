/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/DiskListLocal.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#endif

#include "InDetVertexValidation/VertexEventLoop.h"

int main( int argc, char* argv[] ) {

  // Take the submit directory from the input if provided:
  std::string submitDir = "submitDir";
  if( argc > 1 ) submitDir = argv[ 1 ];

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Construct the samples to run on:
  SH::SampleHandler sh;
  //IF want to scan a directory:
  SH::scanDir( sh, "/afs/cern.ch/work/g/glee/data/Vertex/mc15_14TeV.160024.PowhegPythia8_AU2CT10_VBFH125_gamgam.recon.AOD.e1337_s3142_s3143_r9589/" );
  //single file:
  //SH::DiskListLocal list(".");
  //SH::scanDir( sh, list, "aod.root");

  // Set the name of the input TTree. It's always "CollectionTree"
  // for xAOD files.
  sh.setMetaString( "nc_tree", "CollectionTree" );

  // Print what we found:
  sh.print();

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler( sh );

  // Add our analysis to the job:
  VertexEventLoop * alg = new VertexEventLoop();
  job.algsAdd( alg );

  // Run the job using the local/direct driver:
  EL::DirectDriver driver;
  driver.submit( job, submitDir );

  return 0;

}
