/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// A unit test for the track selection tool. This is far from comprehensive - at this point it is mostly a framework to add tests as new features are added.

// System include(s):
#include <memory>
#include <cstdlib>
#include <cassert>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// Local include(s):
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

int main( int /*argc*/, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   // fail on an unchecked StatusCode
   StatusCode::enableFailure();

   // Initialise the application:
   ASG_CHECK_SA( APP_NAME, static_cast<StatusCode>(xAOD::Init( APP_NAME )) );


   // Create the tool(s) to test:
   InDet::InDetTrackSelectionTool tightSelTool( "TightPrimaryTrackSelection", "TightPrimary" );
   // Configure and initialise the tool(s):
   tightSelTool.msg().setLevel( MSG::VERBOSE );
   ASG_CHECK_SA( APP_NAME, tightSelTool.setProperty("minPt", 100.0) ); // the cuts can be modified from the pre-defined sets
   //ASG_CHECK_SA( APP_NAME, tightSelTool.setProperty("minNTrtHits", 1); )
   ASG_CHECK_SA( APP_NAME, tightSelTool.initialize() );

   InDet::InDetTrackSelectionTool minBiasSelTool( "MinBiasTrackSelection", "MinBias" );
   ASG_CHECK_SA( APP_NAME, minBiasSelTool.initialize() );

   // create a track that should pass:
   std::unique_ptr<xAOD::TrackParticle> goodTrack = std::unique_ptr<xAOD::TrackParticle>(new xAOD::TrackParticle);
   goodTrack->makePrivateStore();
   goodTrack->setDefiningParameters(0., 0., 0., 1.4, 1./15000.);
   goodTrack->setFitQuality( 10., 10 );
   uint8_t uint8_0 = 0;
   uint8_t uint8_1 = 1;
   uint8_t uint8_2 = 2;
   uint8_t uint8_4 = 4;
   uint8_t uint8_8 = 8;
   goodTrack->setSummaryValue(uint8_1, xAOD::numberOfInnermostPixelLayerHits);
   goodTrack->setSummaryValue(uint8_1, xAOD::numberOfNextToInnermostPixelLayerHits);
   goodTrack->setSummaryValue(uint8_4, xAOD::numberOfPixelHits);
   goodTrack->setSummaryValue(uint8_8, xAOD::numberOfSCTHits);
   goodTrack->setFitQuality(10., 10.); // chiSq, nDoF


   assert( tightSelTool.accept(*goodTrack) );
   assert( minBiasSelTool.accept(*goodTrack) );
   
   // make sure the track is accepted with no BLayer or IBL hits when neither are expected
   goodTrack->setSummaryValue(uint8_0, xAOD::expectInnermostPixelLayerHit);
   goodTrack->setSummaryValue(uint8_0, xAOD::expectNextToInnermostPixelLayerHit);
   goodTrack->setSummaryValue(uint8_0, xAOD::numberOfInnermostPixelLayerHits);
   goodTrack->setSummaryValue(uint8_0, xAOD::numberOfNextToInnermostPixelLayerHits);
   assert( tightSelTool.accept(*goodTrack) );
   assert( minBiasSelTool.accept(*goodTrack) );

    std::unique_ptr<xAOD::TrackParticle> badTrack = std::unique_ptr<xAOD::TrackParticle>(new xAOD::TrackParticle);
   badTrack->makePrivateStore();
   *badTrack = *goodTrack;
   badTrack->setSummaryValue(uint8_1, xAOD::numberOfPixelSharedHits);
   badTrack->setSummaryValue(uint8_2, xAOD::numberOfSCTSharedHits);

   assert( !tightSelTool.accept(*badTrack) );
   assert( minBiasSelTool.accept(*badTrack) ); // MinBias should still allow this track to pass
   badTrack->setSummaryValue(uint8_0, xAOD::numberOfSCTSharedHits);
   assert( tightSelTool.accept(*badTrack) );
   badTrack->setDefiningParameters(0., 0., 1.2, 2.9, 1./150000.); // make sure -2.5 < eta < -1.9 and pt > 10 GeV
   badTrack->setSummaryValue(uint8_2, xAOD::numberOfPixelHits);
   badTrack->setSummaryValue(uint8_8, xAOD::numberOfSCTHits);
   assert( !tightSelTool.accept(*badTrack) ); // make sure that 10 Si hits fails at large negative eta

   // check the chi-sq cut
   assert ( minBiasSelTool.accept(*badTrack) );
   badTrack->setFitQuality(100., 10);
   assert( !minBiasSelTool.accept(*badTrack) );

   // Info( APP_NAME, "good track pt: %f" , goodTrack->pt() );
   // Info( APP_NAME, "bad track pt: %f" , badTrack->pt() );

   ASG_CHECK_SA( APP_NAME, tightSelTool.finalize() );
   ASG_CHECK_SA( APP_NAME, minBiasSelTool.finalize() );

   // Return gracefully:
   return 0;
}
