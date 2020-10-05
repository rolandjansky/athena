///////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetTrackSumMomentsTool.cxx
// Implementation file for class JetTrackSumMomentsTool
// Author: James Frost <james.frost@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "JetMomentTools/JetTrackSumMomentsTool.h"

#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteDecorHandle.h"

using xAOD::JetFourMom_t;
//using AODTracking::FourMom_t;
//using xAOD::TrackParticle::FourMom_t;

JetTrackSumMomentsTool::JetTrackSumMomentsTool(const std::string& name)
  : asg::AsgTool(name) {
}

//**********************************************************************

StatusCode JetTrackSumMomentsTool::initialize() {
  ATH_MSG_INFO("Initializing JetTrackSumMomentsTool " << name());
  if ( m_htsel.empty() ) {
    ATH_MSG_INFO("  No track selector.");
  } else {
    ATH_MSG_INFO("  Track selector: " << m_htsel->name());
  }

  ATH_CHECK(m_vertexContainer_key.initialize());
  ATH_CHECK(m_tva_key.initialize());

  if(m_jetContainerName.empty()) {
    ATH_MSG_ERROR("JetTrackSumMomentsTool needs to have its input jet container configured!");
    return StatusCode::FAILURE;
  }
  m_trackSumPtKey = m_jetContainerName + "." + m_trackSumPtKey.key();
  m_trackSumMassKey = m_jetContainerName + "." + m_trackSumMassKey.key();

  ATH_CHECK(m_trackSumPtKey.initialize());
  ATH_CHECK(m_trackSumMassKey.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetTrackSumMomentsTool::decorate(const xAOD::JetContainer& jets) const {
  // Get input vertex collection

  auto handle_v = SG::makeHandle(m_vertexContainer_key);
  if (!handle_v.isValid()){
    ATH_MSG_ERROR("Could not retrieve the VertexContainer: "
                  << m_vertexContainer_key.key());
    return StatusCode::FAILURE;
  }

  auto vertexContainer = handle_v.cptr();

  // Get the track-vertex association
  auto handle_tva = SG::makeHandle(m_tva_key);
  if (!handle_tva.isValid()){
    ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation: "
                  << m_tva_key.key());
    return StatusCode::FAILURE;
  }

  auto tva = handle_tva.cptr();

  SG::WriteDecorHandle<xAOD::JetContainer, float> trackSumPtHandle(m_trackSumPtKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> trackSumMassHandle(m_trackSumMassKey);
  
  // Get the tracks associated to the jet
  // Note that there may be no tracks - this is both normal and an error case
  std::vector<const xAOD::TrackParticle*> tracks;
  for(const xAOD::Jet* jet : jets) {
    if ( ! jet->getAssociatedObjects(m_assocTracksName, tracks) ) {
      ATH_MSG_DEBUG("Associated tracks not found.");
    }
  
    if (vertexContainer->size() == 0 ) { 
      ATH_MSG_WARNING("There are no vertices in the container. Exiting"); 
      return StatusCode::FAILURE;
    }

    const xAOD::Vertex* HSvertex = findHSVertex(vertexContainer);

    const std::pair<float,float> tracksums = getJetTrackSums(HSvertex, tracks, tva);

    trackSumPtHandle(*jet) = tracksums.first;
    trackSumMassHandle(*jet) = tracksums.second;

    ATH_MSG_VERBOSE("JetTrackSumMomentsTool " << name()
                    << ": TrackSumPt=" << tracksums.first
                    << ", TrackSumMass=" << tracksums.second  );
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

const xAOD::Vertex* JetTrackSumMomentsTool::findHSVertex(const xAOD::VertexContainer*& vertices) const
{
  for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ) {
    if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx) {
      
      ATH_MSG_VERBOSE("JetVertexTaggerTool " << name() << " Found HS vertex at index: "<< iVertex);
      return vertices->at(iVertex);
    }
  }
  ATH_MSG_WARNING("There is no vertex of type PriVx. Taking default vertex.");
  return vertices->at(0);
}

//**********************************************************************

std::pair<float,float> JetTrackSumMomentsTool::getJetTrackSums(const xAOD::Vertex* vertex, 
								  const std::vector<const xAOD::TrackParticle*>& tracks, 
								  const jet::TrackVertexAssociation* tva) const {
  
  bool notsel = m_htsel.empty();
  unsigned int nkeep = 0;
  unsigned int nskip = 0; 
  xAOD::TrackParticle::FourMom_t tracksum(0,0,0,0);
  for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack) {
    const xAOD::TrackParticle* track = tracks.at(iTrack);
    if ( notsel || m_htsel->keep(*track) ) {
      const xAOD::Vertex* ptvtx = tva->associatedVertex(track);
      if (!m_requireTrackPV || ptvtx != nullptr ) { // Track has vertex assigned
	// Add to sums
	if (!m_requireTrackPV || ptvtx->index() == vertex->index() ) { tracksum += track->p4(); }
      }
      ++nkeep;
    }
    else { ++nskip; }
  }

  ATH_MSG_VERBOSE("JetTrackSumMomentsTool " << name()
                  << ": nsel=" << nkeep
	 	  << ", nrej=" << nskip           );

  return std::make_pair(tracksum.Pt(),tracksum.M());
  
}

//**********************************************************************
