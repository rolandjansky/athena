/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// ITest.h
//
//  Abstract object that represents a trigger analysis test.
//
#ifndef __ITest__
#define __ITest__

#include "TrigDecisionTool/TrigDecisionTool.h"

namespace TrigAnalysisTest {

  class ITest {
  public:
    virtual ~ITest() {}

    // Called as even event in the file is processed.
    virtual void processEvent(Trig::TrigDecisionTool &trigDecTool) = 0;
    
    // Called at the end of the job - return 0 if the test passed.
    // Otherwise some non-zero integer.
    virtual int finalize() = 0;
  };

}

#endif
