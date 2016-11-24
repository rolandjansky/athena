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
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h" 
#include "MCTruthClassifier/MCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

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
   std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   CHECK( ifile.get() );

   // Create a TEvent object:
   xAOD::TEvent event;
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

   MCTruthClassifier myClassifier ("myClassifier");
   CHECK( myClassifier.initialize() );

   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {

     // Tell the object which entry to look at:
     event.getEntry( entry );

     std::cout << "=================NEXT EVENT==========================" << std::endl;

     //Electrons
     const xAOD::ElectronContainer* electrons = 0 ;
     CHECK( event.retrieve(electrons, "Electrons") );

     xAOD::ElectronContainer::const_iterator el_it      = electrons->begin();
     xAOD::ElectronContainer::const_iterator el_it_last = electrons->end(); 
     unsigned int i = 0; 

     for (; el_it != el_it_last; ++el_it, ++i) {
       const xAOD::Electron* el = (*el_it); 
       std::cout << "Electron " << el << " Num " << i << std::endl;
       std::cout << "xAOD pt = " << (*el_it)->pt() << std::endl;
       Info (APP_NAME,"Electron #%d", i); 
       std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin>
          classification = myClassifier.particleTruthClassifier(*el_it);

       static SG::AuxElement::Accessor<int> tT("truthType") ;
       if (tT.isAvailable(**el_it)){;
          Info (APP_NAME,"Electron Type from  Reco returns %d ", tT(**el_it) );
       }
       Info (APP_NAME,"Electron Type from  Analysis Base returns %d ", classification.first );
       static SG::AuxElement::Accessor<int> tO("truthOrigin") ;
       if (tO.isAvailable(**el_it)){;
          Info (APP_NAME,"Electron Origin from  Reco returns %d ", tO(**el_it) );
       }
       Info (APP_NAME,"Electron Origin from  Analysis Base returns %d ", classification.second );
     }
     Info( APP_NAME, "===>>>  done processing event #%lld ",entry);
   }

   CHECK( myClassifier.finalize() );
   // Return gracefully:
   return 0;
}

//  LocalWords:  const
