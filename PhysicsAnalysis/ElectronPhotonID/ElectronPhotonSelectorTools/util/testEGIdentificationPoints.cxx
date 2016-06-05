/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h" 
#include "xAODEgamma/Electron.h" 
#include "xAODEgamma/Photon.h" 

#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "CxxUtils/make_unique.h"
#include "AsgTools/AsgMessaging.h"

// Derivation include
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"



// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

//Define msg functionality using AsgMessaging
//---------------------------------------------------
namespace{  
  asg::AsgMessaging dummymsg("");
}
#define DUMMY_MSG( lvl, ARG ) {dummymsg.msg(lvl)<<ARG<<endmsg ;}
#define MSG_DEBUG( ARG ) {DUMMY_MSG(MSG::DEBUG, ARG);}
#define MSG_INFO( ARG ) {DUMMY_MSG(MSG::INFO, ARG);}
#define MSG_WARNING( ARG ) {DUMMY_MSG(MSG::WARNING, ARG);}
#define MSG_ERROR( ARG ) {DUMMY_MSG(MSG::ERROR, ARG);}
#define MSG_FATAL( ARG ) {DUMMY_MSG(MSG::FATAL, ARG);}
#define MSG_ABORT( ARG ) {DUMMY_MSG(MSG::FATAL, ARG); std::abort();} 
#define CHECK( ARG )                                                    \
  do {                                                                  \
    const bool result = ARG;                                            \
    if( ! result ) {                                                    \
      MSG_ERROR("FAILED  to execute" <<#ARG);                           \
      return EXIT_FAILURE;                                              \
    }                                                                   \
  } while( false )
//---------------------------------------------------

//main test code
int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   MSG::Level mylevel=MSG::DEBUG;
   dummymsg.msg().setLevel(mylevel);
   dummymsg.msg().setName(APP_NAME);
   // Check if we received a file name:
   if( argc < 2 ) {
     MSG_ERROR("No file name received!" );
     MSG_ERROR( "  Usage: %s [xAOD file name]");
     return EXIT_FAILURE;
   }
   
   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   const TString fileName = argv[ 1 ];
   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   CHECK( ifile.get() );
 
  // Check if we want to process Electron or Photon
   bool isElectron = true;   
   if( argc < 4 ) {
     Info (APP_NAME, "By default lookign to Electron" );
   } else {  
    int  argv1 = atoi(argv[ 3 ]);
    if(argv1 == 0)   isElectron = false; 
    if(isElectron)  Info( APP_NAME, "We are lookign to Electron-ID: %i", isElectron );
    else  Info( APP_NAME, "We are lookign to Photon-ID" );
   }
   // Create a TEvent object (persistent store)

   //For 2.4.5 testing
   xAOD::TEvent pers( xAOD::TEvent::kAthenaAccess );
   //For 2.3
   //xAOD::TEvent pers( xAOD::TEvent::kBranchAccess );

   // Create a TStore object (transient store)
   xAOD::TStore trans;
   CHECK( pers.readFrom( ifile.get() ) );
   //
   MSG_INFO("Number of events in the file: "<< pers.getEntries());

   // Decide how many events to run over:
   Long64_t entries = pers.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }


   if (isElectron) {
   //Medium cut based Electrons
   std::unique_ptr<AsgElectronIsEMSelector> m_electronMediumIsEMSelector = CxxUtils::make_unique<AsgElectronIsEMSelector> ("electronMediumIsEMSelector");
   CHECK(m_electronMediumIsEMSelector->setProperty("WorkingPoint", "MediumElectron"));
   m_electronMediumIsEMSelector->msg().setLevel(mylevel);
   CHECK(m_electronMediumIsEMSelector->initialize());
   
   //Medium Likelihood electron
   std::unique_ptr<AsgElectronLikelihoodTool> m_MediumLH = CxxUtils::make_unique<AsgElectronLikelihoodTool> ("MediumLH");
   CHECK(m_MediumLH->setProperty("WorkingPoint", "MediumLHElectron"));
   m_MediumLH->msg().setLevel(mylevel);
   CHECK(m_MediumLH->initialize());
   
   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     pers.getEntry( entry );
     MSG_INFO("============================");
     MSG_INFO("Event: " <<entry);

     const xAOD::ElectronContainer* electrons;
     CHECK(pers.retrieve(electrons, "Electrons"));

     unsigned int counter=0;
     for (const xAOD::Electron* el : *electrons) {
       MSG_INFO("---------------------------");
       MSG_INFO("Electron: " << counter);
       MSG_INFO("Electron LH Medium accept result: " <<m_MediumLH->accept(*el));
       MSG_INFO("Electron stored LH Medium: "  << el->passSelection("LHMedium") );
       MSG_INFO("Electron Cut Medium accept result: " <<m_electronMediumIsEMSelector->accept(*el));
       MSG_INFO("Electron stored Medium: "  << el->passSelection("Medium") );
       ++counter;
     }
   } // loop entries
   
   } // is electron
   else {
     //Tight cut based photon
     std::unique_ptr<AsgPhotonIsEMSelector> m_photonTightIsEMSelector = CxxUtils::make_unique<AsgPhotonIsEMSelector> ("photonTightIsEMSelector");
     CHECK(m_photonTightIsEMSelector->setProperty("WorkingPoint", "TightPhoton"));
     m_photonTightIsEMSelector->msg().setLevel(mylevel);
     CHECK(m_photonTightIsEMSelector->initialize());
     
     // Loop over the events:
     for( Long64_t entry = 0; entry < entries; ++entry ) {
       
       // Tell the object which entry to look at:
       pers.getEntry( entry );
       MSG_INFO("============================");
       MSG_INFO("Event: " <<entry);
       
       const xAOD::PhotonContainer* photons;
       CHECK(pers.retrieve(photons, "Photons"));
       unsigned int counter=0;
       for (const xAOD::Photon* ph : *photons) {
	 if(ph->author() != xAOD::EgammaParameters::AuthorCaloTopo35){
	   MSG_INFO("---------------------------");
	   MSG_INFO("Photon: " << counter);
	   MSG_INFO("Photon Tight accept result: " <<m_photonTightIsEMSelector->accept(*ph));
	   ++counter;       
	 }
       }
     }// loop entries
   }


   xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();
   return 0;
}



