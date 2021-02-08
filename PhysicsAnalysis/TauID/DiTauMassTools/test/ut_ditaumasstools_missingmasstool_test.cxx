// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <memory>
#include <set>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TTree.h>
#include <TSystem.h>

// Core EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
// Local include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"
#include "DiTauMassTools/MissingMassTool.h"

#include "AsgMessaging/MessageCheck.h"
#include "AsgTools/AnaToolHandle.h"

int main() {

   ANA_CHECK_SET_TYPE (int);
   using namespace asg::msgUserCode;

   // Get the name of the application:
   const char* APP_NAME = "ut_ditaumasstools_missingmasstool_test";

   // Initialise the environment:
   ANA_CHECK( xAOD::Init(APP_NAME) );

   // Initialize the tool
   MissingMassTool missingmassTool("missingmass_tool");
   ANA_CHECK( missingmassTool.setProperty("Decorate", true)) ;
   ANA_CHECK( missingmassTool.setProperty("UseVerbose", 1)) ;
   ANA_CHECK( missingmassTool.setProperty("CalibSet", "2012"));
   ANA_CHECK( missingmassTool.setProperty("alg_version", 3));
   ANA_CHECK( missingmassTool.setProperty("UseTailCleanup", 0));
   ANA_CHECK( missingmassTool.setProperty("NiterFit2", 30));
   ANA_CHECK( missingmassTool.setProperty("NiterFit3", 10));
   ANA_CHECK( missingmassTool.setProperty("UseTauProbability", 0));

   ANA_CHECK( missingmassTool.initialize() );

   // Connect an input file to the event:
   
   const char *FNAME = getenv("ASG_TEST_FILE_MC");

   std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );

   if( ! ifile.get() ) {
     ::Error( APP_NAME, XAOD_MESSAGE( "File %s couldn't be opened..." ),
	      FNAME );
     return 1;
   }

   // Create the TEvent object
   xAOD::TEvent event(xAOD::TEvent::kClassAccess);
   ANA_CHECK( event.readFrom(ifile.get()) );

   int nevents_with_two_taus = 0;
   Long64_t entries = event.getEntries();
   for (Long64_t entry = 0; entry < entries; entry++) {
     if (nevents_with_two_taus > 10)
       break;
     
     event.getEntry(entry);

     // retrieve the EDM objects
     const xAOD::EventInfo * ei = 0;
     ANA_CHECK(event.retrieve(ei, "EventInfo"));
     
     const xAOD::JetContainer *jets = 0;
     ANA_CHECK(event.retrieve(jets, "AntiKt4EMPFlowJets"));

     const xAOD::TauJetContainer *taus = 0;
     ANA_CHECK(event.retrieve(taus, "TauJets"));
     if (taus->size() < 2)
       continue;
     nevents_with_two_taus++;
     const xAOD::MissingETContainer *mets = 0;
     //  Use MET_Core_AntiKt4EMPFlow as proxy of the final MET value
     ANA_CHECK(event.retrieve(mets, "MET_Core_AntiKt4EMPFlow")); 

     int njets_25 = 0;
     xAOD::JetContainer::const_iterator jetItr;
     for(jetItr = jets->begin(); jetItr != jets->end(); ++jetItr) {
       const xAOD::Jet *jet = *jetItr;
       if (jet->pt() > 25000.) 
	 njets_25++;
     }
     const xAOD::MissingET *met = mets->at(mets->size() - 1);
     const xAOD::TauJet *tau1 = taus->at(0);
     const xAOD::TauJet *tau2 = taus->at(1);
     CP::CorrectionCode c = missingmassTool.apply(*ei, tau1, tau2, met, njets_25);
     if (c != CP::CorrectionCode::Ok)
       ::Error(APP_NAME, "Something went wrong in the apply method");

     ::Info(APP_NAME, "%d events processed", (int)entry);
     ::Info(APP_NAME, "mmc maxw = %f GeV", ei->auxdata<double>("mmc_maxw_mass"));
     ::Info(APP_NAME, "mmc mlm = %f GeV", ei->auxdata<double>("mmc_mlm_mass"));
     ::Info(APP_NAME, "mmc mlnu3p = %f GeV", ei->auxdata<double>("mmc_mlnu3p_mass"));

   }

   return 0;
}
