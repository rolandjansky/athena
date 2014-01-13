/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ROOT
#include "TSystem.h"
#include "TROOT.h"

// Trigger
//#include "TrigMonitoringEvent/TrigMonConfig.h"
//#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "PhysicsBase/RunModule.h"

/*
Command to compile executable:

$ g++ src/run_rates.cxx `root-config --libs` `root-config --glibs` `root-config --cflags` -I/afs/cern.ch/user/r/rustem/testarea/AtlasAnalysis/InstallArea/include/PhysicsBase -L/afs/cern.ch/user/r/rustem/testarea/AtlasAnalysis/InstallArea/i686-slc4-gcc34-opt/lib -lPhysicsBase
*/

int main(int argc, char **argv)
{
  TROOT root("simple","Test of histogramming and I/O");
  
  if(argc && argv) {
  }

  //TrigMonEvent event;
  //event.clear();

  //TrigMonConfig config;
  //config.clear();

  gSystem->Load("libTree");
  gSystem->Load("libTrigMonitoringEvent");
  gSystem->Load("libTrigMonitoringEventDict");
  gSystem->Load("libTrigCostRate");

  Anp::RunModule run;
  run.Execute();
  
  return 0;
}
