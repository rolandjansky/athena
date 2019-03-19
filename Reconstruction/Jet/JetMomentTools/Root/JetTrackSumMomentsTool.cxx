///////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetTrackSumMomentsTool.cxx
// Implementation file for class JetTrackSumMomentsTool
// Author: James Frost <james.frost@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "JetMomentTools/JetTrackSumMomentsTool.h"

using xAOD::JetFourMom_t;
//using AODTracking::FourMom_t;
//using xAOD::TrackParticle::FourMom_t;

JetTrackSumMomentsTool::JetTrackSumMomentsTool(const std::string& name)
    : JetModifierBase(name)
      // , m_vertexContainer("")
    , m_assocTracksName("")
    , m_htsel("",this)
    , m_vertexContainer_key("")
    , m_tva_key("")
{
  declareProperty("VertexContainer",m_vertexContainer_key);
  declareProperty("AssociatedTracks",m_assocTracksName);
  declareProperty("TrackVertexAssociation",m_tva_key);
  declareProperty("TrackSelector", m_htsel);
  declareProperty("RequireTrackPV", m_requireTrackPV = true);
}

//**********************************************************************

StatusCode JetTrackSumMomentsTool::initialize() {
  ATH_MSG_DEBUG("initializing version with data handles");
  ATH_MSG_INFO("Initializing JetTrackSumMomentsTool " << name());
  if ( m_htsel.empty() ) {
    ATH_MSG_INFO("  No track selector.");
  } else {
    ATH_MSG_INFO("  Track selector: " << m_htsel->name());
  }

  ATH_CHECK(m_vertexContainer_key.initialize());
  ATH_CHECK(m_tva_key.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetTrackSumMomentsTool::modifyJet(xAOD::Jet& jet) const {

  // Get input vertex collection

  auto handle_v = SG::makeHandle(m_vertexContainer_key);
  if (!handle_v.isValid()){
    ATH_MSG_ERROR("Could not retrieve the VertexContainer: "
                  << m_vertexContainer_key.key());
    return 1;
  }

  auto vertexContainer = handle_v.cptr();

  // Get the track-vertex association
  auto handle_tva = SG::makeHandle(m_tva_key);
  if (!handle_tva.isValid()){
    ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation: "
                  << m_tva_key.key());
    return 2;
  }

  auto tva = handle_tva.cptr();

  // Get the tracks associated to the jet
  // Note that there may be no tracks - this is both normal and an error case
  std::vector<const xAOD::TrackParticle*> tracks;
  if ( ! jet.getAssociatedObjects(m_assocTracksName, tracks) ) {
    ATH_MSG_DEBUG("Associated tracks not found.");
  }
  
  if (vertexContainer->size() == 0 ) { 
    ATH_MSG_WARNING("There are no vertices in the container. Exiting"); 
    return 4;
  }

  const xAOD::Vertex* HSvertex = findHSVertex(vertexContainer);

  const std::pair<float,float> tracksums = getJetTrackSums(HSvertex, tracks, tva);

  jet.setAttribute("TrackSumPt",tracksums.first);
  jet.setAttribute("TrackSumMass",tracksums.second);

  ATH_MSG_VERBOSE("JetTrackSumMomentsTool " << name()
		  << ": TrackSumPt=" << tracksums.first
		  << ", TrackSumMass=" << tracksums.second  );
  return 0;
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
