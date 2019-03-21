/* 
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TrackAssistHelper.cxx

#include "JetRecTools/TrackAssistHelper.h"

TrackAssistHelper::TrackAssistHelper() 
: m_assocTracksInName(""),
  m_vertexColl(""),
  m_trackVtxAssoc(""),
  m_doTrackVtxAssoc(true),
  m_trackSelTool("")
{
}

StatusCode TrackAssistHelper::initialize()
{

  // If no VertexContainer is given, give warning but proceed without it
  if( m_vertexColl.empty() ) {
    ANA_MSG_WARNING("No VertexContainer defined. The TrackVertexAssociation requirement will be turned off to avoid undefined behavior.");
    m_doTrackVtxAssoc = false;
  }
  
  // If no TrackVertexAssociation is given, give warning but proceed without it
  if( m_trackVtxAssoc.empty() ) {
    ANA_MSG_WARNING("No TrackVertexAssociation defined. The TrackVertexAssociation requirement will be turned off to avoid undefined behavior.");
    m_doTrackVtxAssoc = false;
  }
  
  // If TrackVertexAssociation is turned off, give warning but proceed
  if( !m_doTrackVtxAssoc ) {
    ANA_MSG_WARNING("TrackVertexAssociation retquirement will not be applied. This is not recommended for general use.");
  }
  
  // If no TrackSelector is given, give warning but proceed without it
  if( m_trackSelTool.empty() ) {
    ANA_MSG_WARNING("No TrackSelector defined. No track selection will be applied. This is not recommended for general use.");
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackAssistHelper::rescaleTracks( const xAOD::JetContainer *jets, xAOD::TrackParticleContainer *tracks) const {

  for( const xAOD::Jet* jet : *jets ) {
    // Get track rescale weights
    ANA_CHECK( getRescaleFactors(*jet, *tracks) );
  }

  for( xAOD::TrackParticle* track : *tracks ) {
    
    // Get the scales
    float scale = track->auxdata< float >("TAScale");

    // Include this to preserve the pT of any unmatched tracks
    if( !scale ) scale = 1.0;

    // Rescale track pT
    track->setDefiningParameters( track->d0(), track->z0(), track->phi0(), track->theta(), track->qOverP() / scale );

  }

  return StatusCode::SUCCESS;
}

StatusCode TrackAssistHelper::getRescaleFactors( const xAOD::Jet &jet, xAOD::TrackParticleContainer &tracks) const {

  float trackPt = 0;

  // Get weighted sum of track pTs for jet
  for( xAOD::TrackParticle* track : tracks ) {

    // Get associations and weights
    std::vector< std::pair<int, float> > associations = track->auxdata< std::vector< std::pair<int, float> > >("JetAssociations");

    // Loop over associations
    for( std::pair<int, float> association : associations ) {

      // Skip track if it isn't associated to the jet
      if( unsigned(association.first) != jet.index() ) continue;

      // Add weighted pT to total      
      float weight = association.second;
      trackPt += track->pt() * weight;

      // Only add each track once
      break;

    }

  }

  // Save rescale factor for each track
  for( xAOD::TrackParticle* track : tracks ) {

    float scale = 0.0;

    // Get associations and weights
    std::vector< std::pair<int, float> > associations = track->auxdata< std::vector< std::pair<int, float> > >("JetAssociations");

    // Loop over associations
    for( std::pair<int, float> association : associations ) {

      // Skip track if it isn't associated to the jet
      if( unsigned(association.first) != jet.index() ) continue;

      // Get factor to scale 
      float weight = association.second;
      scale = jet.pt() * weight / trackPt;

      // Break loop once association to jet is found
      break;

    }

    // Save the sum of scales
    track->auxdata< float >("TAScale") += scale;

  }

  return StatusCode::SUCCESS;
}

bool TrackAssistHelper::isGoodTrack( const xAOD::TrackParticle &track, const xAOD::Vertex &pvx, const jet::TrackVertexAssociation &tva ) const {

  // Do TrackVertexAssociation check if configured to do so
  if ( m_doTrackVtxAssoc ) {
    // Check if track is associated to primary vertex
    if( &pvx != tva.associatedVertex(&track) ) return false;
  }

  // Do track selection check if configured to do so
  if ( !m_trackSelTool.empty() ) {
    // Check if track passes selection criteria
    if( !m_trackSelTool->keep(track) ) return false;
  }

  return true;
}

StatusCode TrackAssistHelper::getVertexInfo( const xAOD::Vertex *&pvx, const jet::TrackVertexAssociation *&tva ) const {
  
  // Get primary vertex and vertex association if configured to do so
  if( m_doTrackVtxAssoc ) { 
    ANA_CHECK( getPrimaryVertex(pvx) );
    ANA_CHECK( getTrackVertexAssociation(tva) );
  }
  return StatusCode::SUCCESS;
}

void TrackAssistHelper::print() const {
  ANA_MSG_INFO("Properties inherited from TrackAssistHelper");
  ANA_MSG_INFO("   InputAssociatedTracks: " << m_assocTracksInName);
  ANA_MSG_INFO("         VertexContainer: " << m_vertexColl);
  ANA_MSG_INFO("  TrackVertexAssociation: " << m_trackVtxAssoc);
  ANA_MSG_INFO("           TrackSelector: " << m_trackSelTool);
  return;
}
