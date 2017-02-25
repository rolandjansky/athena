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

int main( int argc, char* argv[] ) {

  IAppMgrUI* app = POOL::Init(); //important to do this first!

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // Open the input file:
  TString fileName = "";
  if( argc < 1 ) {
    ANA_MSG_WARNING("Usage: ath_test_truthWewightTool.exe <file>");
  } else {
    fileName = argv[1];
  }
}


