/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

#include <AsgTools/MessageCheck.h>

int main()
{
  using namespace asg::msgUserCode;

  ATH_MSG_INFO ("Unit test for SUSYTools on data");

  // Stored updated references in SUSY ART area for now
  std::string inputFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/data18_13TeV.00348403.physics_Main.merge.AOD.artDAOD.PHYSVAL.pool.root";

  std::string cmd("SUSYToolsTester " + inputFile + " maxEvents=500 isData=1 isAtlfast=0 Debug=0");
  ATH_MSG_INFO ("Will now run this command: " << cmd);
  int ret = system(cmd.c_str());

  if (ret != 0) {
    ATH_MSG_ERROR ("Test failed (return code was " << ret << ")");
    return 1;
  }
  ATH_MSG_INFO ("Finished (return code was " << ret << ")");
  return 0;
}
