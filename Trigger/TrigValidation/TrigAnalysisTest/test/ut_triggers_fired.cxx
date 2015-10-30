/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Test to make sure at least some L1 and some HLT triggers have
// fired.

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

#include "xAODRootAccess/TEvent.h"

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TSystem.h"

#include <iostream>

using namespace std;
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;

// Config
const char* APP_NAME = "trigDecision";

// Helpers
void dumpTriggerInfo(TrigDecisionTool &trigDecisionTool);

int main()
{
  // Initialize (as done for all xAOD standalone programs!)
  if (!xAOD::Init(APP_NAME).isSuccess()) {
    return 1;
  }

  // Load up the proper file we should be checking against.
  auto chain = new TChain("CollectionTree");
  chain->Add(gSystem->Getenv("ROOTCORE_TEST_FILE"));

  // Init data access to the trigger
  TEvent event(TEvent::kBranchAccess);
  if (!event.readFrom(chain).isSuccess()) {
    return 1;
  }

  xAODConfigTool configTool("xAODConfigTool");
  ToolHandle<TrigConf::ITrigConfigTool> configHandle(&configTool);
  configHandle->initialize();
   
  TrigDecisionTool trigDecTool("TrigDecTool");
  trigDecTool.setProperty("ConfigTool",configHandle);
  trigDecTool.setProperty("TrigDecisionKey","xTrigDecision");
  trigDecTool.initialize();

  // Counters
  uint passed_l1 = 0;
  uint passed_hlt = 0;

  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {
    event.getEntry(entry);

    // Did we pass a trigger level?
    if (trigDecTool.isPassed("L1_.*"))
      passed_l1++;
    if (trigDecTool.isPassed("HLT_.*"))
      passed_hlt++;

    // Dump for debugging the test
    if (entry == 1) {
      dumpTriggerInfo(trigDecTool);
    }
  }

  // Dump for debugging.
  cout << "L1 triggers passed: " << passed_l1 << endl;
  cout << "HLT triggers passed; " << passed_hlt << endl;

  // Good run if we found a trigger!
  return (passed_l1 > 0 && passed_hlt > 0) ? 0 : 1;
}

// Dump out all chains in an event
void dumpTriggerInfo(TrigDecisionTool &trigDecTool) {
  cout << "Dumping trigger info for event" << endl;
  auto chainGroups = trigDecTool.getChainGroup(".*");
  for(auto &trig : chainGroups->getListOfTriggers()) {
    cout << "  " << trig << endl;
  }
}

