/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/TrackVertexAssociationTool.h"
#include <cmath>

TrackVertexAssociationTool::TrackVertexAssociationTool(const std::string& t)
    : AsgTool(t)
    , m_tvaTool("") // default as empty
    , m_trackContainer("")
    , m_vertexContainer("")
    , m_tvaStoreName("")
    , m_transDistMax(10e6)
    , m_longDistMax(10e6)
    , m_maxZ0SinTheta(10e6)
{
    declareProperty("TrackParticleContainer",m_trackContainer);
    declareProperty("VertexContainer",m_vertexContainer);
    declareProperty("TrackVertexAssociation",m_tvaStoreName);
    declareProperty("TrackVertexAssoTool", m_tvaTool);

    declareProperty("MaxTransverseDistance",m_transDistMax);
    declareProperty("MaxLongitudinalDistance",m_longDistMax);
    declareProperty("MaxZ0SinTheta", m_maxZ0SinTheta);
    
}

StatusCode TrackVertexAssociationTool::initialize(){
  
  if(! m_tvaTool.empty() ) {
    ATH_MSG_INFO("Intialized using ITrackVertexAssociationTool");
    return m_tvaTool.retrieve();
  }
  ATH_MSG_INFO("Intialized using custom track-vertex association");

  return StatusCode::SUCCESS;
}




int TrackVertexAssociationTool::execute() const {
  // Get input track collection
  const xAOD::TrackParticleContainer* trackContainer = nullptr;
  if ( evtStore()->retrieve(trackContainer, m_trackContainer).isFailure() || trackContainer==nullptr) {
    ATH_MSG_ERROR("Could not retrieve the TrackParticleContainer from evtStore: " << m_trackContainer);
    return 1;
  }

  // If this is a view container, then we assume that it contains elements from only one owning container
  if ( trackContainer->ownPolicy() != SG::OWN_ELEMENTS ) {
    bool oneOwningContainer(true);
    for(const auto& track : *trackContainer) {
      if(track->container() != trackContainer->front()->container()) {
	oneOwningContainer=false;
	break;
      }
    }
    if(!oneOwningContainer) {
      ATH_MSG_ERROR("Track view container holds track from multiple owning containers.");
      ATH_MSG_ERROR("Problem is this container: " << m_trackContainer);
      return 11;
    }
  }

  // Get input vertex collection
  const xAOD::VertexContainer* vertexContainer = nullptr;
  if ( evtStore()->retrieve(vertexContainer,m_vertexContainer).isFailure() || vertexContainer==nullptr) {
    ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: " << m_vertexContainer);
    return 2;
  }

  // Build the TVA
  jet::TrackVertexAssociation* tva;
  
  if(m_tvaTool.empty() )
    tva = buildTrackVertexAssociation_custom(trackContainer,vertexContainer);
  else
    tva = buildTrackVertexAssociation_withTool(trackContainer,vertexContainer);

  // Check the result.
  if ( tva  == nullptr ) {
    ATH_MSG_ERROR("Could not build the TrackVertexAssociation");
    return 3;
  }
  
  // Store it
  if ( evtStore()->record(tva, m_tvaStoreName).isFailure() ) {
    ATH_MSG_ERROR("Unable to write new TrackVertexAssociation to evtStore: " << m_tvaStoreName);
    return 4;
  }

  // Success
  ATH_MSG_DEBUG("Wrote new TrackVertexAssociation to evtStore: " << m_tvaStoreName);
  return 0;
}




//////////////////////////////////////////////////////////////
jet::TrackVertexAssociation* TrackVertexAssociationTool::buildTrackVertexAssociation_withTool(const xAOD::TrackParticleContainer* trackContainer, const xAOD::VertexContainer* vertexContainer) const
{

  ATH_MSG_DEBUG("Building track-vertex association USING InDet tool. trk size="<< trackContainer->size() 
                << "  vtx size="<< vertexContainer->size());

  // Construct object with track container input
  jet::TrackVertexAssociation* tva = new jet::TrackVertexAssociation(trackContainer);

  std::vector<const xAOD::Vertex*> vecVert;
  vecVert.assign(vertexContainer->begin(), vertexContainer->end());

  for( const xAOD::TrackParticle* track : *trackContainer) {
    
    const xAOD::Vertex * v =  m_tvaTool->getUniqueMatchVertex(*track, vecVert) ;
    tva->associate( track, v );
  }
  return tva;
}

//////////////////////////////////////////////////////////////
jet::TrackVertexAssociation* TrackVertexAssociationTool::buildTrackVertexAssociation_custom(const xAOD::TrackParticleContainer* trackContainer, const xAOD::VertexContainer* vertexContainer) const
{
  ATH_MSG_DEBUG("Building track-vertex association trk size="<< trackContainer->size() 
                << "  vtx size="<< vertexContainer->size());
  
  // Construct object with track container input
  jet::TrackVertexAssociation* tva = new jet::TrackVertexAssociation(trackContainer);
  for (size_t iTrack = 0; iTrack < trackContainer->size(); ++iTrack)
    {
      const xAOD::TrackParticle* track = trackContainer->at(iTrack);
      
      // Apply track transverse distance cut
      const float transverseDistance = track->d0();//perigeeParameters().parameters()[Trk::d0];
      if (transverseDistance > m_transDistMax) continue;
        
      
      // Get track longitudinal distance offset
      const float longitudinalDistance = track->z0()+track->vz();
      
      double sinTheta = std::sin(track->theta());

      // For each track, find the vertex with highest sum pt^2 within z0 cut
      size_t matchedIndex = 0;
      bool foundMatch = false;
      for (size_t iVertex = 0; iVertex < vertexContainer->size(); ++iVertex)
        {
          const xAOD::Vertex* vertex = vertexContainer->at(iVertex);
            
          double deltaz = longitudinalDistance - vertex->z();

          // Check longitudinal distance between track and vertex
          if ( fabs(deltaz)  > m_longDistMax)
            continue;

          // Check z0*sinThetha between track and vertex
          if (fabs(deltaz*sinTheta) > m_maxZ0SinTheta)
            continue;

          // If it passed the cuts, then this is the vertex we want
          // This does make the assumption that the container is sorted in sum pT^2 order
          foundMatch = true;
          matchedIndex = iVertex;
          break;
        }

      // If we matched a vertex, then associate that vertex to the track
      if (foundMatch)
        tva->associate(trackContainer->at(iTrack),vertexContainer->at(matchedIndex));
    }


  // Return the TVA object
  return tva;
}
