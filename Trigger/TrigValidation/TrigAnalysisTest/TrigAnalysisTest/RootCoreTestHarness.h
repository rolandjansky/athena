/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Basic RootCore test harness
// Should never be used in an Athena build!
//
#ifndef __RootCoreTestHarness__
#define __RootCoreTestHarness__

#include <string>

namespace TrigAnalysisTest {

  int runTrigAnalysisTest (const std::string &testName);

}

#endif
