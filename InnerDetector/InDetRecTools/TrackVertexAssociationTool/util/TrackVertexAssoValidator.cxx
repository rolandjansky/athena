/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <AsgMessaging/MessageCheck.h>

// EDM include(s):
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Local include(s):
#include "TrackVertexAssociationTool/TrackVertexAssociationTool.h"

int main() {

   using namespace asg::msgUserCode;

   // The name of the application:
   static const char* APP_NAME = "TrackVertexAssoValidator";

   // Set up the environment:
   RETURN_CHECK( APP_NAME, xAOD::Init() );

   // Open the input file:
   std::unique_ptr< TFile > infile( TFile::Open( "$ASG_TEST_FILE_MC",
                                                 "READ" ) );
   if( ( ! infile ) || infile->IsZombie() ) {
      ATH_MSG_ERROR( "Couldn't open the ASG_TEST_FILE_MC file" );
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
   ATH_MSG_INFO( "Total Number of Events: " << nentries );
   Long64_t maxEntries = nentries;
   // run only over the first 100 events
   if(nentries > 100) {
      maxEntries = 100;
   }
   ATH_MSG_INFO( "Running over first " << maxEntries << " events" );
   for( Long64_t entry = 0; entry < maxEntries; ++entry ) {

      // Load the event:
      if( event.getEntry( entry ) < 0 ) {
         ATH_MSG_ERROR( "Couldn't load entry " << entry );
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
         ATH_MSG_WARNING( "Event with no vertex found!" );
         continue;
      }

      // Test isCompitable
      ATH_MSG_INFO("Testing TrackVertexAssociationTool::isCompatible...");
      if(trkCont->size()!=0 && vxCont->size()!=0)
      {
        bool isMatched = trktovxtool.isCompatible(*(trkCont->at(0)), *(vxCont->at(0)));
        ATH_MSG_INFO("Is the first track compatible with the first vertex (the PriVx)? "<< isMatched);
      }

      // Test getMatchMap
      ATH_MSG_INFO("Testing TrackVertexAssociationTool::getMatchMap...");
      xAOD::TrackVertexAssociationMap trkvxassoMap = trktovxtool.getMatchMap(*trkCont, *vxCont);
      ATH_MSG_INFO("Number of vertices for track-vertex association: " << trkvxassoMap.size());
      for (const auto& assoc: trkvxassoMap) {
        const xAOD::Vertex *vx = assoc.first;
        ATH_MSG_INFO("vertex at x, y, z   " << vx->x() << ", " << vx->y() << ", " << vx->z() <<
                     "   has " << assoc.second.size() << " associated tracks");
      }
      // Test getUniqueMatchVertex
      ATH_MSG_INFO("Testing TrackVertexAssociationTool::getUniqueMatchVertex...");
      std::vector<const xAOD::Vertex* > v_vx;
      v_vx.clear();
      for(auto *vertex : *vxCont) {
        v_vx.push_back(vertex);
      }
      if(trkCont->size()!=0)
      {
        const xAOD::Vertex *vx = trktovxtool.getUniqueMatchVertex(*(trkCont->at(0)), v_vx);
        ATH_MSG_INFO("Unique match vertex for first track: " << vx);
      }

      // Test getUniqueMatchVertexLink
      ATH_MSG_INFO("Testing TrackVertexAssociationTool::getUniqueMatchVertexLink...");
      if(trkCont->size() > 2)
      {
        ElementLink<xAOD::VertexContainer> match_vx = trktovxtool.getUniqueMatchVertexLink(*(trkCont->at(2)), *vxCont );

        if(match_vx.isValid())
        {
          ATH_MSG_INFO( "Uniquely matched vertex for third track - ");
          ATH_MSG_INFO( "Vertex ElementLink address: " << match_vx );
          ATH_MSG_INFO( "Vertex address: " << *match_vx );
          ATH_MSG_INFO( "Vertex z pos: " << (*match_vx)->z());
        }
      }

      // Test getUniqueMatchMap
      ATH_MSG_INFO("Testing TrackVertexAssociationTool::getUniqueMatchMap...");
      xAOD::TrackVertexAssociationMap trkvxassoUniqueMap = trktovxtool.getUniqueMatchMap(*trkCont, *vxCont);
      ATH_MSG_INFO("Number of vertices for track-vertex association: " << trkvxassoUniqueMap.size());
      for (const auto& assoc: trkvxassoUniqueMap) {
        const xAOD::Vertex *vx = assoc.first;
        ATH_MSG_INFO("vertex at x, y, z   " << vx->x() << ", " << vx->y() << ", " << vx->z() <<
                     "   has " << assoc.second.size() << " uniquely associated tracks");
      }

   } // end event loop

   // Return gracefully:
   return 0;
}
