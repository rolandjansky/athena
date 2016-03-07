/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Code to run a test of a given name.
//

#include "TrigAnalysisTest/RootCoreTestHarness.h"

#include "TrigAnalysisTest/TestFactory.h"
#include <iostream>

using namespace TrigAnalysisTest;
using namespace std;


#ifdef ROOTCORE
#		include <xAODRootAccess/TEvent.h>
#		include <xAODRootAccess/Init.h>
#		include <xAODRootAccess/tools/ReturnCheck.h>
#               include <AsgTools/SgTEvent.h>
#endif

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TSystem.h"

using namespace std;
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;

namespace TrigAnalysisTest {

  int runTrigAnalysisTest (const std::string &testName)
  {
    // Fetch the test to run it.
    auto test = GetTrigAnalysisTest(testName);
    if (test == nullptr) {
      cout << "Unable to load test" << endl;
      return 1;
    }

    // Initialize (as done for all xAOD standalone programs!)
    if (!xAOD::Init(testName.c_str()).isSuccess()) {
      return 1;
    }

    // Load up the proper file we should be checking against.
    auto chain = new TChain("CollectionTree");
    chain->Add(gSystem->Getenv("ROOTCORE_TEST_FILE"));

    // Init data access to the trigger
    TEvent event(TEvent::kClassAccess);
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

		asg::SgTEvent sgtevent(&event);
		test->setEventStore( &sgtevent );

    // Run the files
    size_t nEntries = chain->GetEntries();
    for (size_t entry = 0; entry < nEntries; entry++) {
      event.getEntry(entry);

      test->processEvent (trigDecTool);
    }

    // Clean up
    return test->finalize();
  }
}
