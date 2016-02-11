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

#include "ElectronPhotonSelectorTools/EGSelectorConfiguration.h"

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
 
   // Create a TEvent object (persistent store)
   xAOD::TEvent pers( xAOD::TEvent::kClassAccess );
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

   //Initialise EG selector
   EGSelectorConfiguration myConfigurator("selectorConfigurator");
   myConfigurator.msg().setLevel(mylevel);
   CHECK(myConfigurator.initialize());
   std::unique_ptr<AsgPhotonIsEMSelector> m_photonTightIsEMSelector = myConfigurator.configuredTool(EgammaSelectors::TightPhoton);
   std::unique_ptr<AsgElectronIsEMSelector> m_electronMediumIsEMSelector = myConfigurator.configuredTool(EgammaSelectors::MediumElectron);
   std::unique_ptr<AsgElectronLikelihoodTool> m_LooseLH = myConfigurator.configuredTool(EgammaSelectors::LooseLHElectron);


   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     pers.getEntry( entry );
     MSG_INFO("============================");
     MSG_INFO("Event: " <<entry);

     const xAOD::ElectronContainer* electrons;
     CHECK(pers.retrieve(electrons, "Electrons"));

     const xAOD::PhotonContainer* photons;
     CHECK(pers.retrieve(photons, "Photons"));

     unsigned int counter=0;
     for (const xAOD::Electron* el : *electrons) {
       MSG_INFO("---------------------------");
       MSG_INFO("Electron: " << counter);
       MSG_INFO("Electron LH Loose accept result: " <<m_LooseLH->accept(*el));
       MSG_INFO("Electron stored LH Loose: "  << el->passSelection("LHLoose") );
       MSG_INFO("Electron Cut Medium accept result: " <<m_electronMediumIsEMSelector->accept(*el));
       MSG_INFO("Electron stored Medium: "  << el->passSelection("Medium") );
       ++counter;
     }

     counter=0;
     for (const xAOD::Photon* ph : *photons) {
       if(ph->author() != xAOD::EgammaParameters::AuthorCaloTopo35){
	 MSG_INFO("---------------------------");
	 MSG_INFO("Photon: " << counter);
	 MSG_INFO("Photon Tight accept result: " <<m_photonTightIsEMSelector->accept(*ph));
	 ++counter;       
       }
     }
   }

   xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();
   CHECK(myConfigurator.finalize());
   return 0;
}


