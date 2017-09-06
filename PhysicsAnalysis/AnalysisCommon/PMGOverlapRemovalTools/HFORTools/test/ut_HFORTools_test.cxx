/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <xAODRootAccess/Init.h>
#include "AsgTools/AnaToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "HFORTools/IHFORSelectionTool.h"

#ifndef XAOD_STANDALONE
#include "POOLRootAccess/TEvent.h"
#endif

#include "TSystem.h"

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

#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif

#ifndef XAOD_STANDALONE

  POOL::TEvent evt(POOL::TEvent::kAthenaAccess);
  if (gSystem->Getenv("ASG_TEST_FILE_MC")){
    evt.readFrom("$ASG_TEST_FILE_MC");
  } else {
    evt.readFrom("/afs/cern.ch/atlas/project/PAT/xAODs/r9315/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1");
    ANA_MSG_INFO("Did not have $ASG_TEST_FILE_MC defined -- normal if not running in an analysis release.  Falling back to another file.");
  }
  evt.getEntry(0);

  //configuring the tool
  asg::AnaToolHandle<IHFORSelectionTool> myTool("HFORSelectionTool/HFORSelectionTool");

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

#endif

  return 0; //zero = success
}
