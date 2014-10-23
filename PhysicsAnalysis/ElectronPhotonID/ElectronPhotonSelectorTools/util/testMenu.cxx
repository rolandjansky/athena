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

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/Egamma.h"

#include "ElectronPhotonSelectorTools/AsgElectronMultiLeptonSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

#include "PathResolver/PathResolver.h"

#define CHECK( ARG )                                  \
do {                                                  \
    const bool result = ARG;                          \
  if( ! result ) {                                    \
      ::Error( APP_NAME, "Failed to execute: \"%s\"", \
#ARG );                                               \
      return 1;                                       \
  }                                                   \
 } while( false )


int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   // Check if we received a file name:
   if( argc < 2 ) {
     Error( APP_NAME, "No file name received!" );
     Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
      return 1;
   }
   
   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );
   
   // Open the input file:
   const TString fileName = argv[ 1 ];
   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   CHECK( ifile.get() );

   // Create a TEvent object:
   xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
   CHECK( event.readFrom( ifile.get() ) );
   Info( APP_NAME, "Number of events in the file: %i",
         static_cast< int >( event.getEntries() ) );


   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }

   // Create a transient object store. Needed for the tools.
   //   xAOD::TStore *m_store;

   //MultiLepton
   AsgElectronMultiLeptonSelector myMultiLepton ("myMultiLepton");
   myMultiLepton.initialize();

   //Likelihood
   AsgElectronLikelihoodTool myLikelihood ("myLikelihood");
   myLikelihood.setProperty("primaryVertexContainer","PrimaryVertices" );
   myLikelihood.setProperty("OperatingPoint",static_cast<unsigned int> (LikeEnum::Loose) );
   myLikelihood.setProperty("inputPDFFileName","ElectronPhotonSelectorTools/v1/ElectronLikelihoodPdfs.root" );
   myLikelihood.initialize();

   AsgElectronIsEMSelector myLoose ("myLoose"); 
   myLoose.setProperty("ConfigFile","ElectronPhotonSelectorTools/SomeTest.conf" );
   myLoose.setProperty("isEMMask",static_cast<unsigned int> (egammaPID::ElectronLoosePP) );
   myLoose.setProperty("PIDName",static_cast<int> (egammaPID::IsEMLoose) );
   myLoose.initialize();

   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     event.getEntry( entry );
     
     std::cout << "=================NEXT EVENT==========================" << std::endl;
     
     const xAOD::EventInfo* event_info = 0;  
     CHECK( event.retrieve( event_info, "EventInfo" ) ); 
      
     const xAOD::ElectronContainer* electrons;  
     CHECK( event.retrieve(electrons, "ElectronCollection") );
     
     auto el_it      = electrons->begin(); 
     auto el_it_last = electrons->end(); 
     unsigned int i = 0; 
     for (; el_it != el_it_last; ++el_it, ++i) { 
       const xAOD::Electron* el = *el_it; 
       std::cout << "Electron " << i << std::endl; 
       std::cout << "xAOD/raw pt = " << el->pt() << std::endl; 
       Info (APP_NAME,"Electron #%d", i); 

       bool value=false;
       //////////////
       if(el->passSelection(value,"MultiLepton")){
	 Info (APP_NAME,"Electron MultiLepton from Reco returns %d ", static_cast<bool> (value) );
       }
       Info (APP_NAME,"Electron MultiLepton accept() returns %d ", static_cast<bool> (myMultiLepton.accept(*el)) );
       ///////////
       if(el->passSelection(value,"LHLoose")){
	 Info (APP_NAME,"Electron Likelihood Loose from Reco returns %d ", static_cast<bool> (value) );
       }
       Info (APP_NAME,"Electron Likelihood Loose accept() returns %d " , static_cast<bool> (myLikelihood.accept(*el)) ); 
       if(el->passSelection(value,"Loose")){
	 Info (APP_NAME,"Electron Loose from Reco returns %d ", static_cast<bool> (value) );
       }
       Info (APP_NAME,"Electron Loose accept() returns %d ", static_cast<bool> (myLoose.accept(*el)) );

     }
     
     Info( APP_NAME,
	   "===>>>  done processing event #%lld ",entry);
     
   }
   myMultiLepton.finalize();
   myLikelihood.finalize();
   myLoose.finalize();
   // Return gracefully:
   return 0;
}
