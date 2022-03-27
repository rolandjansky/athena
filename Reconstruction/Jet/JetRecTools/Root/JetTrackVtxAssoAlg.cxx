/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "JetRecTools/JetTrackVtxAssoAlg.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgDataHandles/WriteHandle.h"


StatusCode JetTrackVtxAssoAlg::initialize() {
  ATH_MSG_DEBUG("Initializing  " );

  ATH_MSG_INFO("Initializing tool " << name() << "...");
  ATH_MSG_DEBUG("initializing version with data handles");
  
  ATH_CHECK(m_trackContainer_key.initialize());
  ATH_CHECK(m_vertexContainer_key.initialize());
  ATH_CHECK(m_tva_key.initialize());

  if(! m_tvaTool.empty() ) {
    ATH_MSG_INFO("Intialized using ITrackVertexAssociationTool");
    return m_tvaTool.retrieve();
  }

  ATH_MSG_INFO("Intialized using custom track-vertex association");
  return StatusCode::SUCCESS;
}


StatusCode JetTrackVtxAssoAlg::execute(const EventContext& ctx) const {

  // Get input track collection

  auto handle_tracks = SG::makeHandle(m_trackContainer_key,ctx);

  if(!handle_tracks.isValid()){
    ATH_MSG_ERROR("Error retrieving TrackParticleContainer from evtStore: " 
                  << m_trackContainer_key.key());
    return StatusCode::FAILURE;
  }

  auto trackContainer = handle_tracks.cptr();

  
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
      ATH_MSG_ERROR("Problem is this container: " << m_trackContainer_key.key());
      return StatusCode::FAILURE;
    }
  }

  // Get input vertex collection
  auto handle_vert = SG::makeHandle(m_vertexContainer_key,ctx);

  if(!handle_vert.isValid()){
    ATH_MSG_ERROR("Error retrieving VertexContainer from evtStore: " 
                  << m_vertexContainer_key.key());
    return StatusCode::FAILURE;
  }

  auto vertexContainer = handle_vert.cptr();

  auto useCustom = m_tvaTool.empty();
  auto tva = makeTrackVertexAssociation(trackContainer,
                                        vertexContainer,
                                        useCustom);
  // Check the result.
  if (!tva){
    ATH_MSG_ERROR("Could not build the TrackVertexAssociation");
    return StatusCode::FAILURE;
  }
  
  // Store it

  auto handle_tva = SG::makeHandle(m_tva_key,ctx);
  if(!handle_tva.record(std::move(tva))){
    ATH_MSG_ERROR("Unable to write new TrackVertexAssociation to evtStore: "
                  << m_tva_key.key());
    return StatusCode::FAILURE;
  }

  // Success
  ATH_MSG_DEBUG("Wrote new TrackVertexAssociation to evtStore: " 
                << m_tva_key.key());
  return StatusCode::SUCCESS;
  
}



std::unique_ptr<jet::TrackVertexAssociation> 
JetTrackVtxAssoAlg::buildTrackVertexAssociation_withTool(const xAOD::TrackParticleContainer* trackContainer, const xAOD::VertexContainer* vertexContainer) const
{

  ATH_MSG_DEBUG("Building track-vertex association USING InDet tool. trk size="<< trackContainer->size() 
                << "  vtx size="<< vertexContainer->size());

  auto tva = std::make_unique<jet::TrackVertexAssociation>(trackContainer);

  std::vector<const xAOD::Vertex*> vecVert;
  vecVert.assign(vertexContainer->begin(), vertexContainer->end());

  for( const xAOD::TrackParticle* track : *trackContainer) {
    
    const xAOD::Vertex * v =  m_tvaTool->getUniqueMatchVertex(*track, vecVert) ;
    tva->associate( track, v );
  }
  return tva;
}

//////////////////////////////////////////////////////////////
// jet::TrackVertexAssociation* 
std::unique_ptr<jet::TrackVertexAssociation> 
JetTrackVtxAssoAlg::buildTrackVertexAssociation_custom(const xAOD::TrackParticleContainer* trackContainer, const xAOD::VertexContainer* vertexContainer) const
{
  ATH_MSG_DEBUG("Building track-vertex association trk size="<< trackContainer->size() 
                << "  vtx size="<< vertexContainer->size());
  
  auto tva = std::make_unique<jet::TrackVertexAssociation>(trackContainer);

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
          if ( std::abs(deltaz)  > m_longDistMax)
            continue;

          // Check z0*sinThetha between track and vertex
          if (std::abs(deltaz*sinTheta) > m_maxZ0SinTheta)
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

using PTPC = const xAOD::TrackParticleContainer*;
using PVC = const xAOD::VertexContainer*;
std::unique_ptr<jet::TrackVertexAssociation> 
JetTrackVtxAssoAlg::makeTrackVertexAssociation(PTPC trackContainer, 
                                                       PVC vertexContainer,
                                                       bool useCustom) const {
  if (useCustom) {
    return buildTrackVertexAssociation_custom(trackContainer,
                                              vertexContainer);
  } else {
    return buildTrackVertexAssociation_withTool(trackContainer,
                                                vertexContainer);
  }
}
