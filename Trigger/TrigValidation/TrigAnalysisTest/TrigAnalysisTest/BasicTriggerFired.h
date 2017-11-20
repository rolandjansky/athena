/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Test that counts the number of triggers fired
//
#ifndef TRIGANALYSISTEST_BASICTRIGGERFIRED_H
#define TRIGANALYSISTEST_BASICTRIGGERFIRED_H

#include "TrigAnalysisTest/ITest.h"


namespace TrigAnalysisTest {

  class BasicTriggerFired : public ITest {
  public:
    BasicTriggerFired();

    void processEvent(Trig::TrigDecisionTool &trigDecTool);
    int finalize();

  private:
    unsigned int m_passed_l1;
    unsigned int m_passed_hlt;
    bool m_first_call;
    std::map<std::string,int> m_trigger_counts;
  };

}

#endif
