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

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODCaloEvent/CaloClusterContainer.h" 
#include "xAODCore/ShallowCopy.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCore/AuxContainerBase.h"

#include "egammaMVACalibAnalysis/egammaMVATool.h"
#include "AsgMessaging/AsgMessaging.h"

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE


//Define msg functionality using AsgMessaging
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

   // Open the output file:
   std::unique_ptr< TFile > ofile( TFile::Open( "DxAOD.pool.root", "RECREATE" ) );
   if( ! ofile.get() ) {
     MSG_ERROR("Couldn't open output file" );
          return EXIT_FAILURE;
   }
 
   // Create a TEvent object (persistent store)
   xAOD::TEvent pers( xAOD::TEvent::kClassAccess );
   // Create a TStore object (transient store)
   xAOD::TStore trans;
   CHECK( pers.readFrom( ifile.get() ) );
   CHECK( pers.writeTo( ofile.get() ) ); 
   
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
   egammaMVATool myMVATool ("myMVATool");
   CHECK(myMVATool.setProperty("folder", "egammaMVACalib/offline/v3_E4crack_bis"));
   CHECK(myMVATool.initialize());

   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     pers.getEntry( entry );
     MSG_INFO("Event: " <<entry);

     const xAOD::EventInfo* event_info = 0;  
     CHECK( pers.retrieve( event_info, "EventInfo" ) ); 


     const xAOD::ElectronContainer* electrons;
     CHECK(pers.retrieve(electrons, "Electrons"));

     const xAOD::CaloClusterContainer* clusters;
     CHECK(pers.retrieve(clusters, "egammaClusters"));

     //Clone electron
     std::pair<xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > 
       electronShallowcopy = xAOD::shallowCopyContainer( *electrons );

     // Deep copy clusters
     xAOD::CaloClusterContainer* clusters_copy = new xAOD::CaloClusterContainer();
     xAOD::AuxContainerBase* clusters_copy_aux = new xAOD::AuxContainerBase();
     clusters_copy->setStore(clusters_copy_aux);
     CHECK(trans.record(clusters_copy,
			"CalibratedClusters") );
     CHECK(trans.record( clusters_copy_aux,
			"CalibratedClustersAux." ) );

     for (xAOD::Electron* el : *electronShallowcopy.first) {
       
       const xAOD::CaloCluster* oldCluster = el->caloCluster();
       xAOD::CaloCluster* newCluster = new xAOD::CaloCluster(*oldCluster);
       CHECK(myMVATool.execute(newCluster,el));
       clusters_copy->push_back(newCluster);
       std::vector<ElementLink<xAOD::CaloClusterContainer>> links_clusters;
       links_clusters.push_back(ElementLink<xAOD::CaloClusterContainer>(newCluster, *clusters_copy));
       el->setCaloClusterLinks(links_clusters);
     }

     //Final saving
     // Copy the original container, and save the shallow copy:
     CHECK( pers.copy( "Electrons" ) );
     CHECK(pers.record( electronShallowcopy.first,
			"CalibratedElectrons" ) );
     CHECK(pers.record( electronShallowcopy.second,
			"CalibratedElectronsAux." ) );

     CHECK(pers.record(clusters_copy,
			"CalibratedClusters") );
     CHECK(pers.record( clusters_copy_aux,
			"CalibratedClustersAux." ) );


     // Record the output event:
     pers.fill();     
     trans.clear();
   }
   
   CHECK(pers.finishWritingTo( ofile.get() ) );


   CHECK(myMVATool.finalize());
   return 0;
}
