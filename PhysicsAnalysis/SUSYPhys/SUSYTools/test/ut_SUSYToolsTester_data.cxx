/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

#include <AsgTools/MessageCheck.h>

int main()
{
  using namespace asg::msgUserCode;

  ATH_MSG_INFO ("Unit test for SUSYTools on data");

  // Full `env` makes log file diffs useless.  Check if the input file changed, though - points us quickly to an issue.
  ATH_MSG_INFO ("Test files");
  system("env | grep ASG_TEST_FILE_ | sort");

  std::string cmd("SUSYToolsTester $ASG_TEST_FILE_DATA maxEvents=500 isData=1 isAtlfast=0 Debug=0");
  ATH_MSG_INFO ("Will now run this command: " << cmd);
  int ret = system(cmd.c_str());

  if (ret != 0) {
    ATH_MSG_INFO ("Test failed (return code was " << ret << ")");
    return 1;
  }
  ATH_MSG_INFO ("Finished (return code was " << ret << ")");
  return 0;
}
