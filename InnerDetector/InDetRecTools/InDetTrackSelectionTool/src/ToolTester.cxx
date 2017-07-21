/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes:
#include "ToolTester.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#ifndef XAOD_ANALYSIS
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#endif

InDet::ToolTester::ToolTester( const std::string& name, ISvcLocator* svcLoc )
  : AthReentrantAlgorithm( name, svcLoc ),
    m_sgKey("InDetTrackParticles"),
    m_selTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this )
{
    
  declareProperty( "SGKey", m_sgKey);

  declareProperty( "TrackSelectionTool", m_selTool );
}

StatusCode InDet::ToolTester::initialize() {

  // Greet the user:
  ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "SGKey = " << m_sgKey );
  ATH_MSG_DEBUG( "TrackSelectionTool  = " << m_selTool );

  // Retrieve the tools:
  ATH_CHECK( m_selTool.retrieve() );

  ATH_CHECK( m_sgKey.initialize() );
  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode InDet::ToolTester::execute_r(const EventContext &ctx) const {

  // Retrieve the tracks:
  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_sgKey,ctx);
  if (!tracks.isValid()) {
    ATH_MSG_ERROR("Invalid xAOD::TrackParticleContainer " << m_sgKey.key());
    return StatusCode::FAILURE;
  }
  Int_t numberOfTracks = tracks->size();
  ATH_MSG_INFO( "Number of tracks: " << numberOfTracks );

  // we will be copying TAccepts to see if the cuts are consistent for each version of accept().
  // this is not how the tool should be used outside of testing, as it is expensive to copy
  // the TAccepts.
  Root::TAccept acceptxAOD;
  Root::TAccept acceptTrk;

  Int_t numberOfGoodTracks = 0;

  // Loop over them:
  for ( const xAOD::TrackParticle* track : *tracks ) {
    // Select "good" tracks:
    if( ! m_selTool->accept( *track , track->vertex() ) ) continue;
    numberOfGoodTracks++;
    acceptxAOD = m_selTool->accept( *track, track->vertex() );
#ifndef XAOD_ANALYSIS // if we are in full athena we have access to Trk::Tracks
     if (track->track()) {
       Trk::Track trkTrack = *(track->track());
       acceptTrk = m_selTool->accept( trkTrack, nullptr );
       assert( acceptxAOD == acceptTrk );
     }
#endif // XAOD_ANALYSIS

    // Print some info about the selected track:
    ATH_MSG_VERBOSE( "  Selected track: eta = " << track->eta()
		     << ", phi = " << track->phi()
		     << ", pt = " << track->pt() );

  }


  // Return gracefully:
  return StatusCode::SUCCESS;
}
