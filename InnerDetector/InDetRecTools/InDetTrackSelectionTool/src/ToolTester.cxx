/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "ToolTester.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#ifndef XAOD_ANALYSIS
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#endif

InDet::ToolTester::ToolTester( const std::string& name, ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_selTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this ) {
    
  declareProperty( "SGKey", m_sgKey = "InDetTrackParticles" );

  declareProperty( "TrackSelectionTool", m_selTool );
}

StatusCode InDet::ToolTester::initialize() {

  // Greet the user:
  ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "SGKey = " << m_sgKey );
  ATH_MSG_DEBUG( "TrackSelectionTool  = " << m_selTool );

  // Retrieve the tools:
  ATH_CHECK( m_selTool.retrieve() );

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode InDet::ToolTester::execute() {

  // Retrieve the tracks:
  const xAOD::TrackParticleContainer* tracks = 0;
  ATH_CHECK( evtStore()->retrieve( tracks, m_sgKey ) );
  Int_t numberOfTracks = tracks->size();
  ATH_MSG_INFO( "Number of tracks: " << numberOfTracks );

  // we will be copying TAccepts to see if the cuts are consistent for each version of accept().
  // this is not how the tool should be used outside of testing, as it is expensive to copy
  // the TAccepts.
  //Root::TAccept acceptxAOD;
  Root::TAccept acceptTrk;

  Int_t numberOfGoodTracks = 0;

  // Loop over them:
  //xAOD::TrackParticleContainer::const_iterator trk_itr = tracks->begin();
  //const xAOD::TrackParticleContainer::const_iterator trk_end = tracks->end();
  //for( ; trk_itr != trk_end; ++trk_itr ) {
  for ( const xAOD::TrackParticle* track : *tracks ) {
    // Select "good" tracks:
    if( ! m_selTool->accept( *track , track->vertex() ) ) continue;
    numberOfGoodTracks++;
    //acceptxAOD = m_selTool->accept( *track, track->vertex() );
#ifndef XAOD_ANALYSIS
     if (track->track()) {
       Trk::Track trkTrack = *(track->track());
       acceptTrk = m_selTool->accept( trkTrack, 0 );
       
       //       if (trkTrack.trackSummary()->get(Trk::numberOfPixelDeadSensors) >= 0)
       //	 ATH_MSG_INFO( "Pixel dead sensors non-negative in Trk::Track" );

       //   if (acceptxAOD != acceptTrk) {
    // 	ATH_MSG_INFO( "Two accept results not the same!" );
    // 	ATH_MSG_INFO( "xAOD: " << acceptxAOD.getCutResultInvertedBitSet() );
    // 	ATH_MSG_INFO( "Trk: " << acceptTrk.getCutResultInvertedBitSet() );
    // 	uint8_t nPixHits = 0;
    // 	uint8_t nSctHits = 0;
    // 	uint8_t nBLayerHits = 0;
    // 	uint8_t nPixDead = 0;
    // 	uint8_t nSctDead = 0;
    // 	uint8_t nBLayerShared = 0;
    // 	uint8_t nPixHoles = 0;
    // 	track->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits);
    // 	track->summaryValue(nPixHits, xAOD::numberOfPixelHits);
    // 	track->summaryValue(nSctHits, xAOD::numberOfSCTHits);	
    // 	track->summaryValue(nPixDead, xAOD::numberOfPixelDeadSensors);	
    // 	track->summaryValue(nSctDead, xAOD::numberOfSCTDeadSensors);	
    // 	track->summaryValue(nBLayerShared, xAOD::numberOfBLayerSharedHits);	
    // 	track->summaryValue(nPixHoles, xAOD::numberOfPixelHoles);	
    // 	ATH_MSG_INFO( "BLayer hits: " << int(nBLayerHits) << " (xAOD)\t" << trkTrack.trackSummary()->get(Trk::numberOfBLayerHits) << " (Trk)" );
    // 	ATH_MSG_INFO( "Pixel hits: " << int(nPixHits) << " (xAOD)\t" << trkTrack.trackSummary()->get(Trk::numberOfPixelHits) << " (Trk)" );
    // 	ATH_MSG_INFO( "Sct hits: " << int(nSctHits) << " (xAOD)\t" << trkTrack.trackSummary()->get(Trk::numberOfSCTHits) << " (Trk)" );
    // 	ATH_MSG_INFO( "Pixel dead: " << int(nPixDead) );
    // 	ATH_MSG_INFO( "Pixel holes: " << int(nPixHoles) );
    // 	ATH_MSG_INFO( "Sct dead: " << int(nSctDead) );
    // 	ATH_MSG_INFO( "BLayer shared: " << int(nBLayerShared) );

    // 	ATH_MSG_INFO( trkTrack );
    // 	if (acceptxAOD.getCutResult("Pt") != acceptTrk.getCutResult("Pt")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in Pt cut." );
    // 	}
    // 	if (acceptxAOD.getCutResult("Eta") != acceptTrk.getCutResult("Eta")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in Eta cut." );
    // 	}
    // 	if (acceptxAOD.getCutResult("Z0") != acceptTrk.getCutResult("Z0")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in Z0 cut." );
    // 	}
    // 	if (acceptxAOD.getCutResult("Z0SinTheta") != acceptTrk.getCutResult("Z0SinTheta")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in Z0SinTheta cut." );
    // 	}
    // 	if (acceptxAOD.getCutResult("PixelHits") != acceptTrk.getCutResult("PixelHits")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in PixelHits cut." );
    // 	}
    // 	if (acceptxAOD.getCutResult("SiHits") != acceptTrk.getCutResult("SiHits")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in SiHits cut." );
    // 	}
    // 	if (acceptxAOD.getCutResult("BLayersHits") != acceptTrk.getCutResult("BLayersHits")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in BLayersHits cut." );
    // 	}
    // 	if (acceptxAOD.getCutResult("SctHits") != acceptTrk.getCutResult("SctHits")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in SctHits cut." );
    // 	}
    // 	if (acceptxAOD.getCutResult("TrtHits") != acceptTrk.getCutResult("TrtHits")) {
    // 	  ATH_MSG_WARNING( "Two versions differ in TrtHits cut." );
    // 	}

    //   } else {
    // 	ATH_MSG_INFO( "results agree." );
    //   }
    // } else {
    //   ATH_MSG_WARNING( "No Trk::Track for xAOD::TrackParticle." );
     }
#endif // XAOD_ANALYSIS

    // Print some info about the selected track:
    ATH_MSG_INFO( "  Selected track: eta = " << track->eta()
		  << ", phi = " << track->phi()
		  << ", pt = " << track->pt() );

  }

  ATH_MSG_INFO( "  Tracks that pass: " << numberOfGoodTracks << "/" << numberOfTracks );

  // Return gracefully:
  return StatusCode::SUCCESS;
}
