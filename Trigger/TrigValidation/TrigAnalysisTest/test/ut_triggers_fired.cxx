/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Test to make sure at least some L1 and some HLT triggers have
// fired.

#include "TrigAnalysisTest/RootCoreTestHarness.h"

int main()
{
  return TrigAnalysisTest::runTrigAnalysisTest("BasicTriggerFired");
}
