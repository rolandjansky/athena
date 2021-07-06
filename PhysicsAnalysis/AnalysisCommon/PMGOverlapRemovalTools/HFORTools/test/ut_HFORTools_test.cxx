/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AsgTools/StandaloneToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "PMGAnalysisInterfaces/IHFORSelectionTool.h"

#ifdef XAOD_STANDALONE
#   include <TFile.h>
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#else
#   include "POOLRootAccess/TEvent.h"
#endif

// System include(s).
#include <memory>

using namespace asg::msgUserCode;

//==============================================================================
// Unit tests to be used in the NICOS system.
// This reads a few EOS files of ALPGEN+PYTHIA6 and non-ALPGEN+PYTHIA6,
// initializes the tool and process a few events.
// TODO: store a comparison record of kill rate based on the sample type
// for checking the correct behavior of the tool.
//==============================================================================
int main() {
  ANA_CHECK_SET_TYPE (int); //makes ANA_CHECK return ints if exiting function

#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());

  std::unique_ptr< TFile > ifile( TFile::Open( "${ASG_TEST_FILE_MC}" ) );
  xAOD::TEvent evt( xAOD::TEvent::kAthenaAccess );
  ANA_CHECK( evt.readFrom( ifile.get() ) );
#else
  //start just by loading the first event of the test MC file
  POOL::TEvent evt(POOL::TEvent::kAthenaAccess);
  ANA_CHECK(evt.readFrom("$ASG_TEST_FILE_MC"));
#endif // XAOD_STANDALONE

  evt.getEntry(0);

  //configuring the tool
  asg::StandaloneToolHandle<IHFORSelectionTool> myTool("HFORSelectionTool/HFORSelectionTool");

  ANA_CHECK(myTool.setProperty("HFORStrategy", "DRBased"));
  ANA_CHECK(myTool.setProperty("MatchCone", 0.4));
  ANA_CHECK(myTool.setProperty("runConfigFile", "HFORTools/mc15_AlpgenPythia_2016.cfg"));
  ANA_CHECK(myTool.setProperty("TruthJetCollectionName", "AntiKt4TruthWZJets" ));

  ANA_CHECK(myTool.initialize());

  //Loop over 5 events
  for (int i=0;i<5;i++) {

	  evt.getEntry(i);

	  const xAOD::EventInfo* eventInfo = 0;
	  ANA_CHECK( evt.retrieve( eventInfo, "EventInfo" ) );
	  unsigned long int evtNumber = eventInfo->eventNumber() ;

	  bool pass = myTool->isSelected() ;

	  if (!pass) {
		  ANA_MSG_INFO(" *** " << i << " Event number " << evtNumber << " to be killed");
	  }
	  else {
		  ANA_MSG_INFO(" *** " << i << " Event passed. Number: " << evtNumber <<
				  (unsigned int) myTool->getDecisionType() );
	  }
  }

  return 0; //zero = success
}
