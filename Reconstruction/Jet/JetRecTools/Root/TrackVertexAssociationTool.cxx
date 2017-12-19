/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/TrackVertexAssociationTool.h"
#include <cmath>

TrackVertexAssociationTool::TrackVertexAssociationTool(const std::string& t)
    : AsgTool(t)
    , m_tvaTool("") // default as empty
    , m_transDistMax(10e6)
    , m_longDistMax(10e6)
    , m_maxZ0SinTheta(10e6)
{
  declareProperty("TrackVertexAssoTool", m_tvaTool);  // Tool handle

  declareProperty("MaxTransverseDistance",m_transDistMax);
  declareProperty("MaxLongitudinalDistance",m_longDistMax);
  declareProperty("MaxZ0SinTheta", m_maxZ0SinTheta);
  
  declareProperty("TrackParticleContainer",m_trackContainer_key);
  declareProperty("VertexContainer",m_vertexContainer_key);
  declareProperty("TrackVertexAssociation",m_tva_key);
    
}

StatusCode TrackVertexAssociationTool::initialize(){
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

int TrackVertexAssociationTool::execute() const {
  // Get input track collection

  auto handle_tracks = SG::makeHandle(m_trackContainer_key);

  if(!handle_tracks.isValid()){
    ATH_MSG_ERROR("Error retrieving TrackParticleContainer from evtStore: " 
                  << m_trackContainer_key.key());
    return 1;
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
      return 11;
    }
  }

  // Get input vertex collection
  auto handle_vert = SG::makeHandle(m_vertexContainer_key);

  if(!handle_vert.isValid()){
    ATH_MSG_ERROR("Error retrieving VertexContainer from evtStore: " 
                  << m_vertexContainer_key.key());
    return 2;
  }

  auto vertexContainer = handle_vert.cptr();

  auto useCustom = m_tvaTool.empty();
  auto tva = makeTrackVertexAssociation(trackContainer,
                                        vertexContainer,
                                        useCustom);
  // Check the result.
  // if ( tva  == nullptr ) {
  if (!tva){
    ATH_MSG_ERROR("Could not build the TrackVertexAssociation");
    return 3;
  }
  
  // Store it

  auto handle_tva = SG::makeHandle(m_tva_key);
  if(!handle_tva.record(std::move(tva))){
    ATH_MSG_ERROR("Unable to write new TrackVertexAssociation to evtStore: "
                  << m_tva_key.key());
    return 4;
  }

  // Success
  ATH_MSG_DEBUG("Wrote new TrackVertexAssociation to evtStore: " 
                << m_tva_key.key());
  return 0;
}




//////////////////////////////////////////////////////////////
// jet::TrackVertexAssociation* 
std::unique_ptr<jet::TrackVertexAssociation> 
TrackVertexAssociationTool::buildTrackVertexAssociation_withTool(const xAOD::TrackParticleContainer* trackContainer, const xAOD::VertexContainer* vertexContainer) const
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
TrackVertexAssociationTool::buildTrackVertexAssociation_custom(const xAOD::TrackParticleContainer* trackContainer, const xAOD::VertexContainer* vertexContainer) const
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

using PTPC = const xAOD::TrackParticleContainer*;
using PVC = const xAOD::VertexContainer*;
std::unique_ptr<jet::TrackVertexAssociation> 
TrackVertexAssociationTool::makeTrackVertexAssociation(PTPC trackContainer, 
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
