/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"
#include "xAODCore/ShallowCopy.h"

#include <iostream>
#include <string>

#define CHECK( ARG )				       \
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

   int OutputLevel = argc > 2 ? atoi(argv[ 2 ]) : 3;
   bool useLogLog  = argc > 3 && atoi(argv[ 3 ]) > 0 ? true : false;
   
   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   const TString fileName = argv[ 1 ];
   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   CHECK( ifile.get() );
   
   // Create a TEvent object:
   //   xAOD::TEvent event( xAOD::TEvent::kBranchAccess ); //will work for a sample produced in devval
   xAOD::TEvent event( xAOD::TEvent::kClassAccess );   
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

   // Initialize photonFS tool
   CP::IsolationCorrectionTool m_isoCorrTool ("isoCorrTool");

   CHECK( m_isoCorrTool.setProperty("IsMC", true) ); //if MC, else false

   //std::string file_unc = PathResolverFindCalibFile("isolation_ptcorrections_rel20_2_v4.root");
   //std::string file_unc ="isolation_ptcorrections_rel20_2_v4.root";
   //CHECK(m_isoCorrTool.setProperty("CorrFile",file_unc));
   CHECK(m_isoCorrTool.setProperty("LogLogFitForLekage",useLogLog));
   CHECK(m_isoCorrTool.setProperty("OutputLevel",OutputLevel));
   
   if(!m_isoCorrTool.initialize()){
     std::cout <<"Failed to initialize the tool, check for errors"<<std::endl;
     return 0;
   }
   
  
   // Loop over the events:
   std::cout << "loop on " << entries << " entries"<<std::endl;
   for( int entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     event.getEntry( entry );   
     
     // Get the EventInfo (run number...):
     const xAOD::EventInfo* ei = 0;
     CHECK( event.retrieve( ei, "EventInfo" ) );
     //std::cout << "Event number = " << ei->eventNumber() << std::endl;
     
     // Get the Photon container from the event:
     const xAOD::PhotonContainer *photons = 0;
     CHECK( event.retrieve( photons, "Photons" ) );
     
     //Clone 
     std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > photons_shallowCopy = xAOD::shallowCopyContainer( *photons );
     
     //Iterate over the shallow copy
     xAOD::PhotonContainer* phsCorr = photons_shallowCopy.first;
     xAOD::PhotonContainer::iterator ph_itr      = phsCorr->begin();
     xAOD::PhotonContainer::iterator ph_end      = phsCorr->end();
     
     unsigned int i = 0;
     for( ; ph_itr != ph_end; ++ph_itr, ++i ) {
       xAOD::Photon* ph = *ph_itr;
       
       // skip photons with pt outsize the acceptance
       if(ph->pt()<10000.0) continue;
       if( fabs(ph->eta())>2.37) continue;
       Info (APP_NAME,"Event #%d, Photon #%d", entry, i); 
       Info (APP_NAME,"xAOD/raw pt = %f, eta = %f isoLeak20 = %f", ph->pt(), ph->eta(), ph->auxdata<float>("topoetcone20ptCorrection") ); 
       
       //dudu
       CHECK(m_isoCorrTool.applyCorrection(*ph));
       Info (APP_NAME,"after the Iso correction isoLeak20 = %f", ph->auxdata<float>("topoetcone20ptCorrection"));
       
       
     }  // END LOOP ON PHOTONS
     
     
     // Get the Electron container from the event:
     const xAOD::ElectronContainer *elecC = 0;
     CHECK( event.retrieve( elecC, "Electrons" ) );
     
     //Clone 
     std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > elecC_shallowCopy = xAOD::shallowCopyContainer( *elecC );
     
     //Iterate over the shallow copy
     xAOD::ElectronContainer* elsCorr = elecC_shallowCopy.first;
     xAOD::ElectronContainer::iterator el_itr      = elsCorr->begin();
     xAOD::ElectronContainer::iterator el_end      = elsCorr->end();
     
     i = 0;
     for( ; el_itr != el_end; ++el_itr, ++i ) {
       xAOD::Electron* ph = *el_itr;
       
       // skip photons with pt outsize the acceptance
       if(ph->pt()<10000.0) continue;
       if( fabs(ph->eta())>2.47) continue;
       Info (APP_NAME,"Event #%d, Electron #%d", entry, i); 
       Info (APP_NAME,"xAOD/raw pt = %f, eta = %f isoLeak20 = %f", ph->pt(), ph->eta(), ph->auxdata<float>("topoetcone20ptCorrection")); 
       
       //dudu
       CHECK(m_isoCorrTool.applyCorrection(*ph));
       Info (APP_NAME,"after the Iso correction isoLeak20 = %f", ph->auxdata<float>("topoetcone20ptCorrection"));
       
       
     }  // END LOOP ON ELECTRONS
     
   } // END LOOP ON EVENTS
   CHECK(m_isoCorrTool.finalize());
   
   // Return gracefully:
   return 1;
} // END PROGRAM
