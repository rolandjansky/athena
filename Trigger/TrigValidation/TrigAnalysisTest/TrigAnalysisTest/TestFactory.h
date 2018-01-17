/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Test Factory - generates a test object given the name of
// a test. Meant to run in both Athena and RootCore environments.
//

#ifndef TRIGANALYSISTEST_TESTFACTORY_H
#define TRIGANALYSISTEST_TESTFACTORY_H

#include "TrigAnalysisTest/ITest.h"

#include <string>

namespace TrigAnalysisTest {

  // Get a test to be run.
  // Exception is thrown if name is not recognized as a valid test.
  ITest *GetTrigAnalysisTest (const std::string &name);

}

#endif
