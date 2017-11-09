/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

int main()
{
  std::cout << "Unit test for SUSYTools on MC" << std::endl;
  std::cout << std::endl;

  // Full `env` makes log file diffs useless.  Check if the input file changed, though - points us quickly to an issue.
  std::cout << std::endl;
  std::cout << "Test files" << std::endl;
  system("env | grep ASG_TEST_FILE_ | sort");

  std::string cmd("SUSYToolsTester $ASG_TEST_FILE_MC maxEvents=100 isData=0 isAtlfast=0 Debug=0 NoSyst=0");
  // pick PRW file based on what release series we have, 20.1 input for 2.3 and 20.7 input for 2.4
  //int releaseSeries = atoi(getenv("ROOTCORE_RELEASE_SERIES"));
  cmd.append(" PRWFile=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/merged_prw_mc16a_latest.root");
  cmd.append(" ilumicalcFile=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ilumicalc_histograms_None_297730-299243.root");

  std::cout << "Will now run this command: " << cmd << std::endl;
  int ret = system(cmd.c_str());

  if (ret != 0) {
    std::cout << "Test failed (return code was " << ret << ")" << std::endl;
    return 1;
  }
  std::cout << "Finished (return code was " << ret << ")" << std::endl;
  return 0;
}
