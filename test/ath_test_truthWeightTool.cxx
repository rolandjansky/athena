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

  // Check that we received at least one file name:
  if( argc < 2 ) {
    ::Error( APP_NAME, "Usage: %s <xAOD file>", APP_NAME );
    return 1;
  }
  
  POOL::TEvent::EReadMode mode = POOL::TEvent::kPOOLAccess; //POOL is slowest, but it can read everything!
  //mode = POOL::TEvent::kClassAccess;
  
  POOL::TEvent evt(mode);
  evt.readFrom( argv[1] );
  
  // Create the truth weight tool:
  xAOD::TruthWeightTool weightTool( "TruthWeightTool" );
  weightTool.setProperty( "OutputLevel", MSG::INFO ).ignore();

  const ::Long64_t Nevts = evt.getEntries();
  for (int i=0;i < Nevts; i++) {
    if ( evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }

    if ( i == 0) {
      auto weightNames = weightTool.getWeightNames();
      auto weights = weightTool.getMCweights();
      for (size_t i=0;i<weightNames.size();++i)
	::Info( APP_NAME,"Weight %3lu has value %.3f and name \"%s\"",
		i,weights[i],weightNames[i].c_str());
    }

    // Give some feedback of where we are:
    if ( (i+1) % 1000 == 0 ) 
      ::Info( APP_NAME, "Processed %i / %llu events",i+1,Nevts);
    
  }

  // Will does this, so, so will I ;p
  app->finalize();
  return 0;
}

#endif
