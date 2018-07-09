/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Used for validation the track vertex association tools in RootCore implementation

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TError.h>

// xAOD include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/tools/ReturnCheck.h"
#endif

// EDM include(s):
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Local include(s):
#include "TrackVertexAssociationTool/TrackVertexAssociationTool.h"

int main() {

   // The name of the application:
   static const char* APP_NAME = "TrackVertexAssoValidator";

   // Set up the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init() );

   // Open the input file:
   std::unique_ptr< TFile > infile( TFile::Open( "$ASG_TEST_FILE_MC",
                                                 "READ" ) );
   if( ( ! infile ) || infile->IsZombie() ) {
      Error( APP_NAME,
             XAOD_MESSAGE( "Couldn't open the ASG_TEST_FILE_MC file" ) );
      return 1;
   }

   // Set up a TEvent object to read from it:
   xAOD::TEvent event;
   RETURN_CHECK( APP_NAME, event.readFrom( infile.get() ) );

   // Set up the needed tool(s):
   CP::TrackVertexAssociationTool
      trktovxtool( "TrackVertexAssociationTool" );
   RETURN_CHECK( APP_NAME, trktovxtool.setProperty( "OutputLevel",
                                                    MSG::ERROR ) );
   RETURN_CHECK( APP_NAME, trktovxtool.setProperty( "WorkingPoint", "Nominal" ) );
   RETURN_CHECK( APP_NAME, trktovxtool.initialize() );

   // Loop over the file:
   const Long64_t nentries = event.getEntries();
   Info( APP_NAME, "Total Number of Events: %lld ", nentries );
   Long64_t maxEntries = nentries;
   // run only over the first 100 events
   if(nentries > 100) {
      maxEntries = 100;
   }
   Info( APP_NAME, "Running over first %lld events", maxEntries );
   for( Long64_t entry = 0; entry < maxEntries; ++entry ) {

      // Load the event:
      if( event.getEntry( entry ) < 0 ) {
         Error( APP_NAME, XAOD_MESSAGE( "Couldn't load entry %lld" ),
                entry );
         return 1;
      }

      // Retrieve the necessary containers:
      const xAOD::TrackParticleContainer *trkCont = 0;
      const xAOD::VertexContainer *vxCont = 0;
      RETURN_CHECK( APP_NAME, event.retrieve( trkCont,
                                              "InDetTrackParticles" ) );
      RETURN_CHECK( APP_NAME, event.retrieve( vxCont, "PrimaryVertices" ) );

      // A sanity check:
      if( ! vxCont->size() ) {
         Warning( APP_NAME, "Event with no vertex found!" );
         continue;
      }

      // Excercise the tool(s):
      xAOD::TrackVertexAssociationMap trktovxmap =
         trktovxtool.getUniqueMatchMap( *trkCont, *vxCont );
      Info( APP_NAME, "Size of TrackVertexAssociationMap for primary vertex: "
            "%lu", trktovxmap[ vxCont->at( 0 ) ].size() );

      ElementLink< xAOD::VertexContainer > match_vx;
      if( trkCont->size() > 2 ) {
         match_vx = trktovxtool.getUniqueMatchVertexLink( *( trkCont->at( 2 ) ),
                                                          *vxCont );
      }
      if( match_vx.isValid() ) {
         Info( APP_NAME, "Vertex assigned to the 3rd track particle:" );
         std::cout << match_vx << std::endl;
         std::cout << *match_vx << std::endl;
         std::cout << ( *match_vx )->z() << std::endl;
      }
   }

   // Return gracefully:
   return 0;
}
