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
    , m_vertexContainer("")
    , m_assocTracksName("")
    , m_tva("")
    , m_htsel("")
{
    declareProperty("VertexContainer",m_vertexContainer);
    declareProperty("AssociatedTracks",m_assocTracksName);
    declareProperty("TrackVertexAssociation",m_tva);
    declareProperty("TrackSelector", m_htsel);
    declareProperty("RequireTrackPV", m_requireTrackPV = true);

    declareProperty("VertexContainer", m_vertexContainer_key);
    declareProperty("AssociatedTracks", m_trackVertexAssoc_key);
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
  ATH_CHECK(m_trackVertexAssoc_key.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************



int JetTrackSumMomentsTool::modifyJet(xAOD::Jet& jet) const {

  // Get input vertex collection
  auto vertexContainer = SG::makeHandle (m_vertexContainer_key);
  if (!vertexContainer.isValid()){
    ATH_MSG_ERROR("Invalid VertexContainer datahandle: " << m_vertexContainer);
    return 1;
  }

  // Get the track-vertex association
  auto tva = SG::makeHandle (m_trackVertexAssoc_key);
  if (!tva.isValid()){
    ATH_MSG_ERROR("Invalid TrackVertexAssociation datahandle: " << m_tva);
    return 2;
  }


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

  const std::pair<float,float> tracksums = getJetTrackSums(HSvertex, 
                                                           tracks, 
                                                           tva);

  jet.setAttribute("TrackSumPt",tracksums.first);
  jet.setAttribute("TrackSumMass",tracksums.second);

  ATH_MSG_VERBOSE("JetTrackSumMomentsTool " << name()
		  << ": TrackSumPt=" << tracksums.first
		  << ", TrackSumMass=" << tracksums.second  );
  return 0;
}


//**********************************************************************

//const xAOD::Vertex* JetTrackSumMomentsTool::findHSVertex(const xAOD::VertexContainer*& vertices) const
const xAOD::Vertex* 
JetTrackSumMomentsTool::findHSVertex(SG::ReadHandle<xAOD::VertexContainer>& 
                                     vertices) const
{

  // data handle points to a collection 
  for (auto vertex : *vertices) {
    if(vertex->vertexType() == xAOD::VxType::PriVtx) {
      ATH_MSG_VERBOSE("JetVertexTaggerTool: found HS vertex");
      return vertex;
    }
  }

  ATH_MSG_WARNING("There is no vertex of type PriVx. Taking default vertex.");
  // what happens if vertices continer is empty? Should give default....
  return (*vertices)[0];
}

//**********************************************************************

std::pair<float,float> 
JetTrackSumMomentsTool::getJetTrackSums(const xAOD::Vertex* vertex, 
                                        const std::vector<const xAOD::TrackParticle*>& tracks, 
                                        SG::ReadHandle<jet::TrackVertexAssociation>& tva) const {
  
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
