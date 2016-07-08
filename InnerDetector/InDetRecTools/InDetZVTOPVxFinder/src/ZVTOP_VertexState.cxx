/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           ZVTOP_VertexState.cxx  -  Description
                             -------------------
    authors : Tatjana Lenz <tatjana.lenz@cern.ch>
 ***************************************************************************/

#include "InDetZVTOPVxFinder/ZVTOP_VertexState.h"
#include "VxVertex/Vertex.h"
#include "TrkTrack/Track.h"

namespace InDet {

  ZVTOP_VertexState::ZVTOP_VertexState():m_vtx_prob(),m_vertex(),m_beam_spot(), m_tracks(0){}

  ZVTOP_VertexState::ZVTOP_VertexState(double& vtx_prob, Trk::Vertex& vertex, bool& beam_spot, std::vector<const Trk::Track*>& tracks): m_vtx_prob(vtx_prob), m_vertex(vertex), m_beam_spot(beam_spot), m_tracks(tracks){}

  ZVTOP_VertexState::ZVTOP_VertexState(const  ZVTOP_VertexState& vs):
  m_vtx_prob(vs.m_vtx_prob),
  m_vertex(vs.m_vertex),
  m_beam_spot(vs.m_beam_spot),
  m_tracks(std::vector<const Trk::Track*>())
  {
         for (std::vector<const Trk::Track*>::const_iterator itr = vs.m_tracks.begin();
         itr != vs.m_tracks.end(); ++itr) m_tracks.push_back(*itr); 
  }
  ZVTOP_VertexState::~ZVTOP_VertexState() {}
} // end of namespace
