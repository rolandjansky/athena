/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>

#include <AsgMessaging/MessageCheck.h>

int main()
{
  using namespace asg::msgUserCode;

  ATH_MSG_INFO ("Unit test for MuonSelectorTools on data");

  // Full `env` makes log file diffs useless.  Check if the input file changed, though - points us quickly to an issue.
  ATH_MSG_INFO ("Test files");
  system("env | grep ASG_TEST_FILE_ | sort");

  std::string cmd("MuonSelectorToolsTester $ASG_TEST_FILE_DATA");
  ATH_MSG_INFO ("Will now run this command: " << cmd);
  int ret = system(cmd.c_str());

  if (ret != 0) {
    ATH_MSG_ERROR ("Test failed (return code was " << ret << ")");
    return 1;
  }

  ATH_MSG_INFO ("Finished (return code was " << ret << ")");
  return 0;
}
