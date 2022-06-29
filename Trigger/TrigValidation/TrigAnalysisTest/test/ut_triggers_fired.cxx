/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Test to make sure at least some L1 and some HLT triggers have
// fired.

#include "TrigAnalysisTest/RootCoreTestHarness.h"
#include <cstdlib>

int main()
{
  setenv ("ROOTCORE_TEST_FILE", getenv ("ASG_TEST_FILE_MC_OLD"), true);
  return TrigAnalysisTest::runTrigAnalysisTest("BasicTriggerFired");
}
