/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetEDM/TrackVertexAssociation.h"
namespace jet {


  TrackVertexAssociation::TrackVertexAssociation(const xAOD::TrackParticleContainer* trkCont)  : 
    m_trackContainer(trkCont) {
    m_vertex.resize(trkCont->size(), 0 );
  }

  void TrackVertexAssociation::associate(const xAOD::TrackParticle* trk, const xAOD::Vertex* vtx){
    m_vertex[ trk->index() ] = vtx;
  }
  
  const xAOD::Vertex* TrackVertexAssociation::associatedVertex(const xAOD::TrackParticle* trk) const {
    return m_vertex[trk->index()];
  }

  std::vector<const xAOD::TrackParticle*> TrackVertexAssociation::associatedTracks(const xAOD::Vertex* vtx) const
  {
    std::vector<const xAOD::TrackParticle*> tracks;
    for (size_t iTrack = 0; iTrack < m_trackContainer->size(); ++iTrack)
      if (associatedVertex(m_trackContainer->at(iTrack))->index() == vtx->index())
        tracks.push_back(m_trackContainer->at(iTrack));

    return tracks;
  }


}
