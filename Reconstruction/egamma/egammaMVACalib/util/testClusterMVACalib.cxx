/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This is an example doing in ROOT what actually EMClusterTool does in reconstruction.

// If you found your way here and you just to correct final objects 
// Please use the nice ElectronPhotonFourMomentumCorrection 

//This is about changing the cluster i.e basic object and
//Should be run in expert mode only

//But since we can do in ROOT what we do in Athena lets do it !!!!

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
#include "xAODCaloEvent/CaloClusterContainer.h" 
#include "xAODCore/ShallowCopy.h"
#include "xAODEgamma/Egamma.h"

#include "egammaMVACalib/egammaMVATool.h"

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


   egammaMVATool myMVATool ("myMVATool");
   myMVATool.initialize();


   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     event.getEntry( entry );
     
     std::cout << "=================NEXT EVENT==========================" << std::endl;
     
     const xAOD::EventInfo* event_info = 0;  
     CHECK( event.retrieve( event_info, "EventInfo" ) ); 

     //Retrieve the original containers
     const xAOD::ElectronContainer* electrons;  
     CHECK( event.retrieve(electrons, "ElectronCollection") );

     const xAOD::CaloClusterContainer* egClusters;  
     CHECK( event.retrieve(egClusters, "egClusterCollection") );


     // Create a transient object store. 
     xAOD::TStore m_store;

     //Make shallow copy as what we going to do is non-const for sure
     std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_copy = xAOD::shallowCopyContainer( *electrons );
     std::pair< xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer* > egClusters_copy = xAOD::shallowCopyContainer( *egClusters );
     m_store.record( electrons_copy.first, "CalibratedElectrons" ) ;
     m_store.record( electrons_copy.second, "CalibratedElectronsAux" );
     m_store.record( egClusters_copy.first, "CalibratedEgClusters" );
     m_store.record( egClusters_copy.second, "CalibratedEgClustersAux"  );

     //Prepare to loop
     auto el_it      = (electrons_copy.first)->begin(); 
     auto el_it_last = (electrons_copy.first)->end(); 
     typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;
     unsigned int i = 0; 
     for (; el_it != el_it_last; ++el_it, ++i) { 

       xAOD::Electron* el = *el_it; 
       std::cout << "Electron " << i << std::endl; 
       std::cout << "Electron Energy " << el->e() << std::endl;
       std::cout << "Electron Cluster Energy " << el->caloCluster()->e() << std::endl; 
       //Get the element link to the cluster
       const ClusterLink_t caloLink = el->caloClusterLink();
       //The copy has the same index, get the corresponding copy
       xAOD::CaloCluster* newCluster = (egClusters_copy.first)->at(caloLink.index());
       std::cout << "Original Cluster Energy " << newCluster->e() << std::endl; 	 
       //Calibrate
       myMVATool.execute(newCluster,el);
       std::cout << "Calibrated Cluster Energy " << newCluster->e() << std::endl; 
       //
       /*
       //Create element link
       ClusterLink_t clusterLink;
       clusterLink.toIndexedElement(*(egClusters_copy.first),caloLink.index());
       const std::vector<ClusterLink_t> clusterLinks{clusterLink};
       //Push back element link
       el->setCaloClusterLinks( clusterLinks );
       std::cout << "Corrected Electron Cluster Energy " << el->caloCluster()->e() << std::endl; 
       std::cout << "Corrected Electron Energy " << el->e() << std::endl;
       //For fun set el pt 
       el->setPt(el->caloCluster()->e()/cosh(el->trackParticle()->eta()));
       std::cout << "Electron Energy " << el->e() << std::endl;
       */
       }
     

     Info( APP_NAME,
	   "===>>>  done processing event #%lld ",entry);
     
   }



   myMVATool.finalize();
   // Return gracefully:
   return 0;
}
