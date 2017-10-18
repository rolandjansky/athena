/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/// Example standalone executable using POOL to read an xAOD
/// Tests the PMGTruthWeightTool, showing example usage

/// @author: James Robinson

// EDM include(s):
#include "AsgTools/MessageCheck.h"
#include "AsgTools/AnaToolHandle.h"

// Project dependent include(s)
#ifdef XAOD_STANDALONE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#else
#include "POOLRootAccess/TEvent.h"
#endif

// Local include(s):
#include "PMGTools/PMGTruthWeightTool.h"
#include "PMGTools/PMGSherpaVjetsSysTool.h"

// ROOT include(s):
#include "TFile.h"

using namespace asg::msgUserCode;  // messaging

int main(int argc, char *argv[])
{
  ANA_CHECK_SET_TYPE (int); // makes ANA_CHECK return ints if exiting function

  // Initialise the application:
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
  ANA_CHECK(xAOD::Init());
#else
  IAppMgrUI *app = POOL::Init();
#endif

  // Use a default MC file for testing if none is provided
  // TString fileName = "/eos/atlas/user/j/jvasquez/public/mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.merge.DAOD_HIGG1D1.e5607_s2726_r7772_r7676_p3015/DAOD_HIGG1D1.10654168._000034.pool.root.1";
  // ... or one with TruthMetaData
  // TString fileName = "/afs/cern.ch/user/s/schaffer/work-sch/public/data/mc16_13TeV.344235.PowhegPy8EG_NNPDF30_AZNLOCTEQ6L1_VBFH125.DAOD.e5500_e5984_s3126_r9781_r9778_21.2_r05_EXT0/user.schaffer/user.schaffer.12283539.EXT0._000004.DAOD_HIGG2D1.pool.root";
  TString fileName = "$ASG_TEST_FILE_MC";
  if (argc < 2) {
    ANA_MSG_WARNING("No file name received, using ASG_TEST_FILE_MC");
  } else {
    fileName = argv[1]; // use the user provided file
  }

  // Initialise TEvent reading
#ifdef XAOD_STANDALONE
  std::unique_ptr<TFile> ptrFile(TFile::Open(fileName, "READ"));
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  ANA_CHECK(event.readFrom(ptrFile.get()));
#else
  POOL::TEvent event(POOL::TEvent::kClassAccess);
  event.readFrom(fileName);
#endif
  ANA_MSG_INFO("Opened file: " << fileName);

  // Create the truth weight tool:
  ANA_MSG_INFO("Creating PMGTruthWeightTool...");
#ifdef XAOD_STANDALONE
  ANA_MSG_INFO("Creating PMGTruthWeightTool...");
  asg::AnaToolHandle< PMGTools::IPMGTruthWeightTool > weightTool;
  ASG_SET_ANA_TOOL_TYPE(weightTool, PMGTools::PMGTruthWeightTool);
  weightTool.setName("PMGTruthWeightTool");
  ANA_CHECK(weightTool.retrieve());
#else
  asg::AnaToolHandle< PMGTools::IPMGTruthWeightTool > weightTool("PMGTools::PMGTruthWeightTool/PMGTruthWeightTool");
  ANA_CHECK(weightTool.initialize());
#endif

  // Loop over a few events:
  ANA_MSG_INFO("Preparing to loop over events...");
  const Long64_t entries = 20;
  for(Long64_t entry = 0; entry < entries; ++entry) {
    if (event.getEntry(entry) < 0) { ANA_MSG_ERROR("Failed to read event " << entry); continue; }

    auto weightNames = weightTool->getWeightNames();
    auto weights = weightTool->getWeights();
    ANA_MSG_INFO("Event #" << entry << ": found " << weights.size() << " weights and " << weightNames.size() << " weight names for this event");

    // Print out the first weight for every event
    if (weightNames.size() > 1) {
      static std::string weight_name_to_test = weightNames.at(1);
      ANA_MSG_INFO("Event #" << entry << ": weight called '" << weight_name_to_test << "' = " << weightTool->getWeight(weight_name_to_test));
    }

    // Full print out for some of the events
    if ((entry + 1) % 5 == 0) {
      // Print out all weights and names
      ANA_MSG_INFO("Printing all " << weights.size() << " weights for this event...");
      for (size_t i = 0; i < weightNames.size(); ++i) {
        ANA_MSG_INFO("   weight " << i << " has name \"" << weightNames[i].c_str() << "\" and value " << weights[i]);
      }

      // Give feedback about where we are
      ANA_MSG_INFO("Processed " << entry + 1 << " / " << entries << " events");
    }
  }

  // trigger finalization of all services and tools created by the Gaudi Application
#ifndef XAOD_STANDALONE
  app->finalize();
#endif

return 0;


  // ANA_CHECK_SET_TYPE (int); // makes ANA_CHECK return ints if exiting function

  // // Initialise the application:
  // IAppMgrUI *app = POOL::Init(); //important to do this first!

  // // Create the truth weight tool:
  // ANA_MSG_INFO("Creating PMGTruthWeightTool...");
  // PMGTools::PMGTruthWeightTool weightTool("PMGTruthWeightTool");
  // ANA_CHECK(weightTool.setProperty("OutputLevel", MSG::DEBUG));
  // ANA_CHECK(weightTool.sysInitialize()); // must call sysInitialize to get the callbacks registered properly for an AsgMetadataTool
  // ANA_CHECK(weightTool.initialize()); // must call sysInitialize to get the callbacks registered properly for an AsgMetadataTool

  // // Use a default MC file for testing if none is provided
  // // TString fileName = "/eos/atlas/user/j/jvasquez/public/mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.merge.DAOD_HIGG1D1.e5607_s2726_r7772_r7676_p3015/DAOD_HIGG1D1.10654168._000034.pool.root.1";
  // // ... or one with TruthMetaData
  // TString fileName = "/afs/cern.ch/user/s/schaffer/work-sch/public/data/mc16_13TeV.344235.PowhegPy8EG_NNPDF30_AZNLOCTEQ6L1_VBFH125.DAOD.e5500_e5984_s3126_r9781_r9778_21.2_r05_EXT0/user.schaffer/user.schaffer.12283539.EXT0._000004.DAOD_HIGG2D1.pool.root";

  // if (argc < 2) {
  //   ANA_MSG_WARNING("No file name received, using default");
  // } else {
  //   fileName = argv[1]; // use the user provided file
  // }

  // // Initialise TEvent reading
  // POOL::TEvent event(POOL::TEvent::kClassAccess); // Class Access is faster than full POOL Access
  // event.readFrom(fileName);
  // ANA_MSG_INFO("Opened file: " << fileName);

  // // Loop over a few events:
  // ANA_MSG_INFO("Preparing to loop over events...");
  // const Long64_t entries = 20;

  // for(Long64_t entry = 0; entry < entries; ++entry) {
  //   if (event.getEntry(entry) < 0) { ANA_MSG_ERROR("Failed to read event " << entry); continue; }

  //   auto weightNames = weightTool.getWeightNames();
  //   auto weights = weightTool.getWeights();
  //   ANA_MSG_INFO("Event #" << entry << ": found " << weights.size() << " weights and " << weightNames.size() << " weight names for this event");

  //   // Print out the first weight for every event
  //   if (weightNames.size() > 1) {
  //     static std::string weight_name_to_test = weightNames.at(1);
  //     ANA_MSG_INFO("Event #" << entry << ": weight called '" << weight_name_to_test << "' = " << weightTool.getWeight(weight_name_to_test));
  //   }

  //   // Full print out for some of the events
  //   if ((entry + 1) % 5 == 0) {
  //     // Print out all weights and names
  //     ANA_MSG_INFO("Printing all " << weights.size() << " weights for this event...");
  //     for (size_t i = 0; i < weightNames.size(); ++i) {
  //       ANA_MSG_INFO("   weight " << i << " has value " << weights[i] << " and name \"" << weightNames[i].c_str() << "\"");
  //     }

  //     // Give feedback about where we are
  //     ANA_MSG_INFO("Processed " << entry + 1 << " / " << entries << " events");
  //   }
  // }

  // // trigger finalization of all services and tools created by the Gaudi Application
  // app->finalize();
  // return 0;
}
