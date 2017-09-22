/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

int main()
{
  std::cout << "Unit test for SUSYTools on MC" << std::endl;
  std::cout << std::endl;
  std::cout << "Result of 'ls'" << std::endl;
  system("ls");
  system("echo PWD=$PWD");
  
  std::cout << "Result of 'ls SUSYTools/data'" << std::endl;
  system("ls $ROOTCOREBIN/../SUSYTools/data");

  std::cout << std::endl;
  std::cout << "list of RootCore/lib" << std::endl;
  system("ls $ROOTCOREDIR/lib");

  std::cout << std::endl;
  std::cout << "Environment variables" << std::endl;
  system("env");

  std::string cmd("SUSYToolsTester $ASG_TEST_FILE_MC maxEvents=100 isData=0 isAtlfast=0 Debug=0 NoSyst=0");
  // pick PRW file based on what release series we have, 20.1 input for 2.3 and 20.7 input for 2.4
  //int releaseSeries = atoi(getenv("ROOTCORE_RELEASE_SERIES"));
  cmd.append(" PRWFile=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/merged_prw_mc15c_latest.root");

  cmd.append(" ilumicalcFile=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root,/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ilumicalc_histograms_None_297730-299243.root");

  std::cout << "Will now run this command: " << cmd << std::endl;
  int ret = system(cmd.c_str());

  if (ret != 0) {
    std::cout << "Test failed (return code was " << ret << ")" << std::endl;
    return 1;
  }
  std::cout << "Finished (return code was " << ret << ")" << std::endl;
  return 0;
}
