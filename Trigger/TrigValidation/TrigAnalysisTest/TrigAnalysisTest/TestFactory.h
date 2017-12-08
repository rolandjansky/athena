/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Test Factory - generates a test object given the name of
// a test. Meant to run in both Athena and RootCore environments.
//

#ifndef __TestFactory__
#define __TestFactory__

#include "TrigAnalysisTest/ITest.h"

#include <string>

namespace TrigAnalysisTest {

  // Get a test to be run.
  // Exception is thrown if name is not recognized as a valid test.
  ITest *GetTrigAnalysisTest (const std::string &name);

}

#endif
