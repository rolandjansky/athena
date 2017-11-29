/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Basic trigger test - look for some number of triggers
// that have actually fired.
//

#include "TrigAnalysisTest/BasicTriggerFired.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include <iostream>

using namespace std;
using namespace Trig;
using namespace xAOD;

namespace {
  // Dump out all chains in an event
  void dumpTriggerInfo(TrigDecisionTool &trigDecTool) {
    cout << "Dumping trigger info for event" << endl;
    auto chainGroups = trigDecTool.getChainGroup(".*");
    for(auto &trig : chainGroups->getListOfTriggers()) {
      cout << "  " << trig << endl;
    }
  }
}

namespace TrigAnalysisTest {
  //
  // Init counters and get ready to run.
  //
  BasicTriggerFired::BasicTriggerFired()
    : m_passed_l1(0),
      m_passed_hlt(0),
      m_first_call(true)
  {
  }

  // Actually process an event.
  void BasicTriggerFired::processEvent( TrigDecisionTool &trigDecTool) {

    // Did we pass a trigger level?
    if (trigDecTool.isPassed("L1_.*"))
      m_passed_l1++;
    if (trigDecTool.isPassed("HLT_.*"))
      m_passed_hlt++;

    // Dump for debugging the test
    if (m_first_call) {
      dumpTriggerInfo(trigDecTool);
      m_first_call = false;
    }
    auto chainGroups = trigDecTool.getChainGroup(".*");
    for(auto &trig : chainGroups->getListOfTriggers()) {
      if(!m_trigger_counts.count(trig))
        m_trigger_counts[trig] = 0;
      if(trigDecTool.isPassed(trig))
        m_trigger_counts[trig] += 1;
    }
  }

  // Dump everything and return a status.
  int BasicTriggerFired::finalize()
  {
    // Dump for debugging.
    cout << "L1 triggers passed: " << m_passed_l1 << endl;
    cout << "HLT triggers passed; " << m_passed_hlt << endl;
    cout << "START SUMMARY: BasicTriggerFired" << endl;
		for (auto it = m_trigger_counts.begin(); it!=m_trigger_counts.end(); it++)
			cout << (*it).first << "\t" << (*it).second <<endl;
    cout << "END SUMMARY  : BasicTriggerFired" << endl;

    // Good run if we found a trigger!
    return (m_passed_l1 > 0 && m_passed_hlt > 0) ? 0 : 1;
  }
}
