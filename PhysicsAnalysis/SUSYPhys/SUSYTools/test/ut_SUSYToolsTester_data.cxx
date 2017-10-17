/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

int main()
{
  std::cout << "Unit test for SUSYTools on data" << std::endl;
  std::cout << std::endl;

  // Full `env` makes log file diffs useless.  Check if the input file changed, though - points us quickly to an issue.
  std::cout << std::endl;
  std::cout << "Test files" << std::endl;
  system("env | grep ASG_TEST_FILE_ | sort");

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
