#ifndef ROOTCORE

/// Example standalone executable using POOL to read an xAOD
/// Tests the TruthWeightTool ... shows example usage

/// Author: Dag Gillberg - based on
///  https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/AnalysisCommon/CPAnalysisExamples/trunk/test/ut_ath_checkTrigger_test.cxx
/// by Will Buttinger
/// Updated for git-based athena by James Robinson

#include "AthAnalysisBaseComps/AthAnalysisHelper.h" //tool creation and configuration
#include "POOLRootAccess/TEvent.h" //event looping
#include "GaudiKernel/ToolHandle.h" //for better working with tools

#include "AsgTools/MessageCheck.h" //messaging
using namespace asg::msgUserCode;  //messaging

//EDM includes

//ROOT includes
#include "TString.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include "TH1D.h"
#include "TFile.h"

//specific includes for this test
#include "TruthWeightTools/TruthWeightTool.h"

int main(int argc, char *argv[])
{

  IAppMgrUI *app = POOL::Init(); //important to do this first!

  // The application's name:
  const char *APP_NAME = argv[0];

  // Use default MC file for testing if none is provided
  TString fileName = "$ASG_TEST_FILE_MC";
  if (argc < 2) {
    ANA_MSG_WARNING("No file name received, using $ASG_TEST_FILE_MC");
  } else {
   fileName = argv[1]; //use the user provided file
  }

  POOL::TEvent::EReadMode mode = POOL::TEvent::kClassAccess; //Class Access is faster than full POOL Access

  POOL::TEvent evt(mode);
  evt.readFrom(fileName);
  //evt.setEvtSelProperty("ReadMetaDataWithPool",false); // If test xAOD format metadata, will need to uncomment this until pool converter is present

  ANA_MSG_INFO("Will create tool");
  // Create the truth weight tool:
  xAOD::TruthWeightTool weightTool("TruthWeightTool");
  weightTool.setProperty("OutputLevel", MSG::DEBUG).ignore();
  weightTool.sysInitialize().ignore(); //must call sysInitialize to get the callbacks registered properly for an AsgMetadataTool

  // Optional: create ToolHandle
  // constructor argument is: Type/Name
  //  ToolHandle< xAOD::ITruthWeightTool > handle( "xAOD::TruthWeightTool/TruthWeightTool" );
  //if ( handle.retrieve().isFailure() ) {
  //  ::Error( APP_NAME, "Could not retrieve TruthWeightTool");
  //  return 1;
  //}

  ANA_MSG_INFO("Will loop");

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
