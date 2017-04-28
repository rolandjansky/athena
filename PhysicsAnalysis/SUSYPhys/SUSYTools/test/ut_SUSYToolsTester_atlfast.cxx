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

  // pick input file based on what release series we have, 20.1 input for 2.3 and 20.7 input for 2.4
  //int releaseSeries = atoi(getenv("ROOTCORE_RELEASE_SERIES"));

  int ret=0;

  //don't run the rest in 2.4.X series until we get an AtlFast sample in 20.7
  ret = system("SUSYToolsTester $ASG_TEST_FILE_MC_AFII 100 isData=0 isAtlfast=1 Debug=0 NoSyst=0");

  if (ret != 0) {
    std::cout << "Test failed (return code was " << ret << ")" << std::endl;
    return 1;
  }

  std::cout << "Finished (return code was " << ret << ")" << std::endl;
  return 0;
}
