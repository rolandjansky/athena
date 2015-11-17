/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Test that counts the number of triggers fired
//
#ifndef __BasicTriggerFired__
#define __BasicTriggerFired__

#include "TrigAnalysisTest/ITest.h"

using namespace std;

namespace TrigAnalysisTest {

  class BasicTriggerFired : public ITest {
  public:
    BasicTriggerFired();

    void processEvent(Trig::TrigDecisionTool &trigDecTool);
    int finalize();

  private:
    unsigned int _passed_l1;
    unsigned int _passed_hlt;
    bool _first_call;
		map<string,int> trigger_counts;
  };

}

#endif
