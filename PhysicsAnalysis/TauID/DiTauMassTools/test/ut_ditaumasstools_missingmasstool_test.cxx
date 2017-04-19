// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Message.h"
#include "DiTauMassTools/MissingMassTool.h"

// Error checking macro
#define CHECK( ARG )							\
  do {                                                                  \
    const bool result = ARG;						\
    if(!result) {							\
      ::Error(APP_NAME, "Failed to execute: \"%s\"",			\
	      #ARG );							\
      return 1;								\
    }									\
  } while( false )

int main() {

   // Get the name of the application:
   const char* APP_NAME = "ut_ditaumasstools_missingmasstool_test";

   // Initialise the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );

   // Initialize the tool
   MissingMassTool missingmassTool("missingmass_tool");
   CHECK( missingmassTool.setProperty("Decorate", true)) ;
   CHECK( missingmassTool.setProperty("UseVerbose", 1)) ;
   CHECK( missingmassTool.setProperty("CalibSet", "2012"));
   CHECK( missingmassTool.setProperty("alg_version", 3));
   CHECK( missingmassTool.setProperty("UseTailCleanup", 0));
   CHECK( missingmassTool.setProperty("NiterFit2", 30));
   CHECK( missingmassTool.setProperty("NiterFit3", 10));
   CHECK( missingmassTool.setProperty("UseTauProbability", 0));

   CHECK( missingmassTool.initialize() );

   // Connect an input file to the event:
   const char *FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r7725/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7725_r7676/AOD.07915862._000100.pool.root.1";

   std::unique_ptr< ::TFile > ifile( ::TFile::Open( FNAME, "READ" ) );

   if( ! ifile.get() ) {
     ::Error( APP_NAME, XAOD_MESSAGE( "File %s couldn't be opened..." ),
	      FNAME );
     return 1;
   }

   // Create the TEvent object
   xAOD::TEvent event(xAOD::TEvent::kClassAccess);
   CHECK( event.readFrom(ifile.get()) );

   int nevents_with_two_taus = 0;
   Long64_t entries = event.getEntries();
   for (Long64_t entry = 0; entry < entries; entry++) {
     if (nevents_with_two_taus > 10)
       break;
     
     event.getEntry(entry);

     // retrieve the EDM objects
     const xAOD::EventInfo * ei = 0;
     CHECK(event.retrieve(ei, "EventInfo"));
     
     const xAOD::JetContainer *jets = 0;
     CHECK(event.retrieve(jets, "AntiKt4LCTopoJets"));

     const xAOD::TauJetContainer *taus = 0;
     CHECK(event.retrieve(taus, "TauJets"));
     if (taus->size() < 2)
       continue;
     nevents_with_two_taus++;
     const xAOD::MissingETContainer *mets = 0;
     CHECK(event.retrieve(mets, "MET_Reference_AntiKt4LCTopo"));

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
