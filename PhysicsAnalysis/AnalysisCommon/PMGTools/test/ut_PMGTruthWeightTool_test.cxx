/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/// Example standalone executable using POOL to read an xAOD
/// Tests the PMGTruthWeightTool, showing example usage

/// @author: Dag Gillberg: based on ut_ath_checkTrigger_test.cxx by Will Buttinger
/// @author: James Robinson: updated for git-based athena

// EDM includes
#include "AsgTools/MessageCheck.h" // messaging
#include "POOLRootAccess/TEvent.h" // event looping

// Local include(s):
#include "PMGTools/PMGTruthWeightTool.h"

using namespace asg::msgUserCode;  //messaging

int main(int argc, char *argv[])
{
  // Initialise the application:
  IAppMgrUI *app = POOL::Init(); //important to do this first!

  // Create the truth weight tool:
  ANA_MSG_INFO("Creating PMGTruthWeightTool...");
  PMGTools::PMGTruthWeightTool weightTool("PMGTruthWeightTool");
  ANA_CHECK(weightTool.setProperty("OutputLevel", MSG::DEBUG));
  ANA_CHECK(weightTool.sysInitialize()); // must call sysInitialize to get the callbacks registered properly for an AsgMetadataTool

  // Use a default MC file for testing if none is provided
  TString fileName = "/eos/atlas/user/j/jvasquez/public/mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.merge.DAOD_HIGG1D1.e5607_s2726_r7772_r7676_p3015/DAOD_HIGG1D1.10654168._000034.pool.root.1";
  if (argc < 2) {
    ANA_MSG_WARNING("No file name received, using default");
  } else {
    fileName = argv[1]; // use the user provided file
  }

  // Initialise TEvent reading
  POOL::TEvent event(POOL::TEvent::kClassAccess); // Class Access is faster than full POOL Access
  event.readFrom(fileName);
  ANA_MSG_INFO("Opened file: " << fileName);

  // Loop over a few events:
  ANA_MSG_INFO("Preparing to loop over events...");
  const Long64_t entries = 20;

  for(Long64_t entry = 0; entry < entries; ++entry) {
    if (event.getEntry(entry) < 0) { ANA_MSG_ERROR("Failed to read event " << entry); continue; }

    auto weightNames = weightTool.getWeightNames();
    auto weights = weightTool.getWeights();
    ANA_MSG_INFO("Event #" << entry << ": found " << weights.size() << " weights and " << weightNames.size() << " weight names for this event");

    // Print out the first weight for every event
    if (weightNames.size() > 1) {
      static std::string weight_name_to_test = weightNames.at(1);
      ANA_MSG_INFO("Event #" << entry << ": weight called '" << weight_name_to_test << "' = " << weightTool.getWeight(weight_name_to_test));
    }

    // Full print out for some of the events
    if ((entry + 1) % 5 == 0) {
      // Print out all weights and names
      ANA_MSG_INFO("Printing all " << weights.size() << " weights for this event...");
      for (size_t i = 0; i < weightNames.size(); ++i) {
        ANA_MSG_INFO("   weight " << i << " has value " << weights[i] << " and name \"" << weightNames[i].c_str() << "\"");
      }

      // Give feedback about where we are
      ANA_MSG_INFO("Processed " << entry + 1 << " / " << entries << " events");
    }
  }

  // trigger finalization of all services and tools created by the Gaudi Application
  app->finalize();
  return 0;
}
