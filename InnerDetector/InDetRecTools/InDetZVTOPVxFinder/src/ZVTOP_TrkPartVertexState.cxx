/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           ZVTOP_TrkPartVertexState.cxx  -  Description
                             -------------------
    authors : Tatjana Lenz <tatjana.lenz@cern.ch>
 ***************************************************************************/

#include "InDetZVTOPVxFinder/ZVTOP_TrkPartVertexState.h"
#include "VxVertex/Vertex.h"
#include "Particle/TrackParticle.h"

namespace InDet {

  ZVTOP_TrkPartVertexState::ZVTOP_TrkPartVertexState():m_vtx_prob(),m_vertex(),m_IP(), m_tracks(0){}

  ZVTOP_TrkPartVertexState::ZVTOP_TrkPartVertexState(double& vtx_prob, Trk::Vertex& vertex, bool& IP, std::vector<const Rec::TrackParticle*>& tracks): m_vtx_prob(vtx_prob), m_vertex(vertex), m_IP(IP), m_tracks(tracks){}

  ZVTOP_TrkPartVertexState::ZVTOP_TrkPartVertexState(const  ZVTOP_TrkPartVertexState& vs):
  m_vtx_prob(vs.m_vtx_prob),
  m_vertex(vs.m_vertex),
  m_IP(vs.m_IP),
  m_tracks(std::vector<const Rec::TrackParticle*>())
  {
         for (std::vector<const Rec::TrackParticle*>::const_iterator itr = vs.m_tracks.begin();
         itr != vs.m_tracks.end(); ++itr) m_tracks.push_back(*itr); 
  }
  ZVTOP_TrkPartVertexState::~ZVTOP_TrkPartVertexState() {}
} // end of namespace
