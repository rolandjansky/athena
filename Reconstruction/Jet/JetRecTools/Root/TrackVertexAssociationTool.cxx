/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/TrackVertexAssociationTool.h"


TrackVertexAssociationTool::TrackVertexAssociationTool(const std::string& t)
    : AsgTool(t)
    , m_trackContainer("")
    , m_vertexContainer("")
    , m_tvaStoreName("")
    , m_transDistMax(0)
    , m_longDistMax(0)
{
    declareProperty("TrackParticleContainer",m_trackContainer);
    declareProperty("VertexContainer",m_vertexContainer);
    declareProperty("TrackVertexAssociation",m_tvaStoreName);
    declareProperty("MaxTransverseDistance",m_transDistMax);
    declareProperty("MaxLongitudinalDistance",m_longDistMax);
}


const jet::TrackVertexAssociation* TrackVertexAssociationTool::buildTrackVertexAssociation(const xAOD::TrackParticleContainer* trackContainer, const xAOD::VertexContainer* vertexContainer) const
{
  ATH_MSG_DEBUG("Building track-vertex association trk size="<< trackContainer->size() 
                << "  vtx size="<< vertexContainer->size());
  
  // Construct object with track container input
    jet::TrackVertexAssociation* tva = new jet::TrackVertexAssociation(trackContainer);
    for (size_t iTrack = 0; iTrack < trackContainer->size(); ++iTrack)
    {
        const xAOD::TrackParticle* track = trackContainer->at(iTrack);

        // Apply track transverse distance cut
        const float transverseDistance = track->perigeeParameters().parameters()[Trk::d0];
        if (transverseDistance > m_transDistMax)
            continue;

        // Get track longitudinal distance offset
        //const float longitudinalDistance = track->perigeeParameters().parameters()[Trk::z0];
        const float longitudinalDistance = track->z0()+track->vz();

        // For each track, find the vertex with highest sum pt^2 within z0 cut
        size_t matchedIndex = 0;
        bool foundMatch = false;
        for (size_t iVertex = 0; iVertex < vertexContainer->size(); ++iVertex)
        {
            const xAOD::Vertex* vertex = vertexContainer->at(iVertex);

            // Check longitudinal distance between track and vertex
            if (fabs(longitudinalDistance - vertex->z()) > m_longDistMax)
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

int TrackVertexAssociationTool::execute() const {
  // Get input track collection
  const xAOD::TrackParticleContainer* trackContainer = nullptr;
  if ( evtStore()->retrieve(trackContainer, m_trackContainer).isFailure() || trackContainer==nullptr) {
    ATH_MSG_ERROR("Could not retrieve the TrackParticleContainer from evtStore: " << m_trackContainer);
    return 1;
  }

  // Check this is not a view container.
  if ( trackContainer->ownPolicy() != SG::OWN_ELEMENTS ) {
    ATH_MSG_ERROR("Track container must hold track directly. View container is not allowed.");
    ATH_MSG_ERROR("Problem is this container: " << m_trackContainer);
    return 11;
  }

  // Get input vertex collection
  const xAOD::VertexContainer* vertexContainer = nullptr;
  if ( evtStore()->retrieve(vertexContainer,m_vertexContainer).isFailure() || vertexContainer==nullptr) {
    ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: " << m_vertexContainer);
    return 2;
  }

  // Build the TVA
  const jet::TrackVertexAssociation* tva = buildTrackVertexAssociation(trackContainer,vertexContainer);
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


