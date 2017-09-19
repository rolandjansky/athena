/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

int main()
{
  std::cout << "Unit test for SUSYTools on data" << std::endl;
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

  std::string cmd("SUSYToolsTester $ASG_TEST_FILE_DATA maxEvents=500 isData=1 isAtlfast=0 Debug=0");
  std::cout << "Will now run this command: " << cmd << std::endl;
  int ret = system(cmd.c_str());

  if (ret != 0) {
    std::cout << "Test failed (return code was " << ret << ")" << std::endl;
    return 1;
  }
  std::cout << "Finished (return code was " << ret << ")" << std::endl;
  return 0;
}
