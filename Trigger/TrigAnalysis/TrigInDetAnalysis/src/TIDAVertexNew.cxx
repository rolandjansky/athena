/**
 **     @file    TIDAVertexNew.cxx
 **
 **     @author  emil haines
 **     @date    Wed  28 Jul 2021
 **
 **     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 **/

#include "TrigInDetAnalysis/TIDAVertexNew.h"


TIDA::VertexNew::VertexNew( 
    const TIDA::Vertex& v, 
    const TrackTrigObject& tobject, 
    const std::vector<TIDA::Track*>* refTracks )
    : TIDA::Vertex(v) {

  const std::vector<unsigned long>& ids = tobject.children();
  m_ids = ids;
  if ( refTracks ) selectTracks( refTracks );
}


TIDA::VertexNew::VertexNew( 
    const TIDA::VertexNew& v, 
    const std::vector<TIDA::Track*>* refTracks,
    const TrackTrigObject* tobject )
    : TIDA::Vertex( v ),
      m_tracks( v.tracks() ), 
      m_ids( v.ids() ) {

  if ( tobject ) {
    const std::vector<unsigned long>& ids = tobject->children();
    m_ids = ids;
  }

  if ( refTracks ) selectTracks( refTracks );
}


void TIDA::VertexNew::selectTracks( 
    const std::vector<TIDA::Track*>* refTracks,
    const TrackTrigObject* tobject ) {

  if ( tobject ) {
    const std::vector<unsigned long>& ids = tobject->children();
    m_ids = ids;
  }

  if ( m_ids.size() == 0 ) {
    return;
  }

  m_tracks.clear();
  selectTracksInternal( refTracks );   
}


void TIDA::VertexNew::selectTracksInternal( 
    const std::vector<TIDA::Track*>* refTracks ) {

  int trackcount = 0;
  for ( size_t iid = 0; iid < m_ids.size(); iid++ ) {
    for (unsigned itr = 0; itr < refTracks->size(); itr++) {
      TIDA::Track* tr = (*refTracks)[itr];
      if ( tr->id() == m_ids[iid] ) {
        addTrack( tr );
        trackcount++;
        break;
      }
    }
  }
  setNtracks( trackcount );    
}