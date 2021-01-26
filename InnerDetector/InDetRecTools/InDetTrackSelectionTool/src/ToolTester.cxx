/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes:
#include "ToolTester.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
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

  ATH_CHECK( m_vertexKey.initialize() );
  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode InDet::ToolTester::execute(const EventContext &ctx) const {

  // Retrieve the tracks:
  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_sgKey,ctx);
  if (!tracks.isValid()) {
    ATH_MSG_ERROR("Invalid xAOD::TrackParticleContainer " << m_sgKey.key());
    return StatusCode::FAILURE;
  }
  Int_t numberOfTracks = tracks->size();
  ATH_MSG_INFO( "Number of tracks: " << numberOfTracks );


  // retrieve vertex container to look for tracks
  SG::ReadHandle<xAOD::VertexContainer> vertices { m_vertexKey, ctx };
  if (!vertices.isValid()) {
    ATH_MSG_ERROR("Invalid xAOD::VertexContainer " << m_vertexKey.key() );
    return StatusCode::FAILURE;
  }

  Int_t numberOfGoodTracks = 0;

  // Loop over them:
  for ( const xAOD::TrackParticle* track : *tracks ) {
    const xAOD::Vertex* foundVertex = nullptr;
    for (const auto vertex : *vertices)
    {
      for (const auto& tpLink : vertex->trackParticleLinks())
      {
	if (*tpLink == track)
	{
	  foundVertex = vertex;
	  break;
	}
      }
      if (foundVertex) break;
    }
    // Select "good" tracks:
    if( ! m_selTool->accept( *track , foundVertex ) ) continue;
    numberOfGoodTracks++;
    asg::AcceptData acceptxAOD = m_selTool->accept( *track, foundVertex );
    ATH_MSG_VERBOSE( " TrackParticle AcceptData to bool " << static_cast<bool>(acceptxAOD));

#ifndef XAOD_ANALYSIS // if we are in full athena we have access to Trk::Tracks
     if (track->track()) {
       Trk::Track trkTrack = *(track->track());
       asg::AcceptData acceptTrk = m_selTool->accept( trkTrack, nullptr );   
       ATH_MSG_VERBOSE( " Trk::Track AcceptData to bool " << static_cast<bool>(acceptTrk));
       assert( static_cast<bool>(acceptxAOD) == static_cast<bool>(acceptTrk) );
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
