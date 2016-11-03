/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

int main()
{
  std::cout << "Unit test for JetTileCorrectionTool on Data" << std::endl;
  std::cout << std::endl;

  std::string cmd = "JetTileCorrectionTester $ASG_TEST_FILE_DATA 50";

  int ret = system(cmd.c_str());

  if (ret != 0) {
    std::cout << "Test failed (return code was " << ret << ")" << std::endl;
    return 1;
  }
  std::cout << "Finished (return code was " << ret << ")" << std::endl;
  return 0;
}

