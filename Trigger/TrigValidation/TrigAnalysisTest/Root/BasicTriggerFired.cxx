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
    : _passed_l1(0),
      _passed_hlt(0),
      _first_call(true)
  {
  }

  // Actually process an event.
  void BasicTriggerFired::processEvent( TrigDecisionTool &trigDecTool) {

    // Did we pass a trigger level?
    if (trigDecTool.isPassed("L1_.*"))
      _passed_l1++;
    if (trigDecTool.isPassed("HLT_.*"))
      _passed_hlt++;

    // Dump for debugging the test
    if (_first_call) {
      dumpTriggerInfo(trigDecTool);
      _first_call = false;
    }
    auto chainGroups = trigDecTool.getChainGroup(".*");
    for(auto &trig : chainGroups->getListOfTriggers()) {
      if(!trigger_counts.count(trig))
				trigger_counts[trig] = 0;
			if(trigDecTool.isPassed(trig))
				trigger_counts[trig] += 1;
    }
  }

  // Dump everything and return a status.
  int BasicTriggerFired::finalize()
  {
    // Dump for debugging.
    cout << "L1 triggers passed: " << _passed_l1 << endl;
    cout << "HLT triggers passed; " << _passed_hlt << endl;
    cout << "START SUMMARY: BasicTriggerFired" << endl;
		for (auto it = trigger_counts.begin(); it!=trigger_counts.end(); it++)
			cout << (*it).first << "\t" << (*it).second <<endl;
    cout << "END SUMMARY  : BasicTriggerFired" << endl;

    // Good run if we found a trigger!
    return (_passed_l1 > 0 && _passed_hlt > 0) ? 0 : 1;
  }
}
