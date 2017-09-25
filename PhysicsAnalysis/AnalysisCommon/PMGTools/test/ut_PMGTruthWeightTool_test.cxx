/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/// Example standalone executable using POOL to read an xAOD
/// Tests the PMGTruthWeightTool, showing example usage

/// @author: Dag Gillberg: based on ut_ath_checkTrigger_test.cxx by Will Buttinger
/// @author: James Robinson: updated for git-based athena

#ifndef ROOTCORE

// ROOT include(s):
#include "TString.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include "TH1D.h"
#include "TFile.h"

// EDM includes
#include "AthAnalysisBaseComps/AthAnalysisHelper.h" // tool creation and configuration
#include "POOLRootAccess/TEvent.h"                  // event looping
#include "GaudiKernel/ToolHandle.h"                 // for better working with tools
#include "AsgTools/MessageCheck.h"                  // messaging

// Local include(s):
#include "PMGTools/PMGTruthWeightTool.h"

using namespace asg::msgUserCode;  //messaging

int main(int argc, char *argv[])
{
  // Initialise application and retrieve its name:
  IAppMgrUI *app = POOL::Init(); //important to do this first!
  const char *APP_NAME = argv[0];

  // Use default MC file for testing if none is provided
  TString fileName = "/eos/atlas/user/j/jvasquez/public/mc15_13TeV.343981.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_gamgam.merge.DAOD_HIGG1D1.e5607_s2726_r7772_r7676_p3015/DAOD_HIGG1D1.10654168._000034.pool.root.1";
  if (argc < 2) {
    ANA_MSG_WARNING("No file name received, using " << fileName);
  } else {
    fileName = argv[1]; // use the user provided file
  }

  // Initialise TEvent reading
  POOL::TEvent::EReadMode mode = POOL::TEvent::kClassAccess; // Class Access is faster than full POOL Access
  POOL::TEvent evt(mode);
  evt.readFrom(fileName);

  // Create the truth weight tool:
  ANA_MSG_INFO("Creating tool...");
  PMGTools::PMGTruthWeightTool weightTool("PMGTruthWeightTool");
  weightTool.setProperty("OutputLevel", MSG::DEBUG).ignore();
  weightTool.sysInitialize().ignore(); //must call sysInitialize to get the callbacks registered properly for an AsgMetadataTool

  ANA_MSG_INFO("Preparing to loop over events...");
  const ::Long64_t Nevts = evt.getEntries();

  for (int i = 0; i < 10; i++) {
    if (evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }

    if (i == 0) {
      ANA_MSG_INFO("Will print");
      auto weightNames = weightTool.getWeightNames();
      auto weights = weightTool.getWeights();

      for (size_t i = 0; i < weightNames.size(); ++i) {
        ::Info(APP_NAME, "Weight %3lu has value %.3f and name \"%s\"", i, weights[i], weightNames[i].c_str());
      }
    }

    // Give some feedback of where we are:
    if ((i + 1) % 1000 == 0) {
      ::Info(APP_NAME, "Processed %i / %llu events", i + 1, Nevts);
    }
  }

  // Will does this, so, so will I ;p
  app->finalize();
  return 0;
}

#endif
