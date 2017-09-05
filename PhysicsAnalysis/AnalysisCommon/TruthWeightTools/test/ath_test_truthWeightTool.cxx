#ifndef ROOTCORE

/// Example standalone executable using POOL to read an xAOD
/// Tests the TruthWeightTool ... shows example usage

/// Author: Dag Gillberg - based on
///  https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/AnalysisCommon/CPAnalysisExamples/trunk/test/ut_ath_checkTrigger_test.cxx
/// by Will Buttinger

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
  const char *APP_NAME = argv[ 0 ];

  // Check that we received at least one file name:
  if (argc < 2) {
    ::Error(APP_NAME, "Usage: %s <xAOD file>", APP_NAME);
    return 1;
  }


  POOL::TEvent::EReadMode mode = POOL::TEvent::kClassAccess; //Class Access is faster than full POOL Access

  POOL::TEvent evt(mode);
  evt.readFrom(argv[1]);
  //evt.setEvtSelProperty("ReadMetaDataWithPool",false); // If test xAOD format metadata, will need to uncomment this until pool converter is present



  ::Info(APP_NAME, "Will create tool");
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

  ::Info(APP_NAME, "Will loop");

  const ::Long64_t Nevts = evt.getEntries();

  for (int i = 0; i < 10; i++) {
    if (evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }

    if (i == 0) {
      ::Info(APP_NAME, "Will print");
      auto weightNames = weightTool.getWeightNames();
      auto weights = weightTool.getWeights();

      for (size_t i = 0; i < weightNames.size(); ++i)
        ::Info(APP_NAME, "Weight %3lu has value %.3f and name \"%s\"",
               i, weights[i], weightNames[i].c_str());
    }

    // Give some feedback of where we are:
    if ((i + 1) % 1000 == 0)
    { ::Info(APP_NAME, "Processed %i / %llu events", i + 1, Nevts); }

  }

  // Will does this, so, so will I ;p
  app->finalize();
  return 0;
}

#endif
