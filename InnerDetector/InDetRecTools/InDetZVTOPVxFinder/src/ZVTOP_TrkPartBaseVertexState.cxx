/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
     ZVTOP_TrkPartBaseVertexState.cxx  -  Description
                             -------------------
    authors : Tatjana Lenz <tatjana.lenz@cern.ch>
 ***************************************************************************/
#include "InDetZVTOPVxFinder/ZVTOP_TrkPartBaseVertexState.h"
#include "VxVertex/Vertex.h"
#include "TrkParticleBase/TrackParticleBase.h"

namespace InDet {

  ZVTOP_TrkPartBaseVertexState::ZVTOP_TrkPartBaseVertexState():m_vtx_prob(),m_vertex(),m_beam_spot(), m_tracks(0){}

  ZVTOP_TrkPartBaseVertexState::ZVTOP_TrkPartBaseVertexState(double& vtx_prob, Trk::Vertex& vertex, bool& beam_spot, std::vector<const Trk::TrackParticleBase*>& tracks): m_vtx_prob(vtx_prob), m_vertex(vertex), m_beam_spot(beam_spot), m_tracks(tracks){}

  ZVTOP_TrkPartBaseVertexState::ZVTOP_TrkPartBaseVertexState(const  ZVTOP_TrkPartBaseVertexState& vs):
  m_vtx_prob(vs.m_vtx_prob),
  m_vertex(vs.m_vertex),
  m_beam_spot(vs.m_beam_spot),
  m_tracks(std::vector<const Trk::TrackParticleBase*>())
  {
         for (std::vector<const Trk::TrackParticleBase*>::const_iterator itr = vs.m_tracks.begin();
         itr != vs.m_tracks.end(); ++itr) m_tracks.push_back(*itr); 
  }
  ZVTOP_TrkPartBaseVertexState::~ZVTOP_TrkPartBaseVertexState() {}
} // end of namespace
