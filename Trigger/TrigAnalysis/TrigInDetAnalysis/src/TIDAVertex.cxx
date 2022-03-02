/**
 **     @file    TIDAVertex.cxx
 **
 **     @author  emil haines, mark sutton
 **     @date    Wed  28 Jul 2021
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/

#include "TrigInDetAnalysis/TIDAVertex.h"

ClassImp(TIDA::Vertex)


TIDA::Vertex::Vertex( 
    double x, double y, double z,
    double dx, double dy, double dz, 
    int Ntracks, double c2, int dof, 
    const std::vector<TIDA::Track*>* tracks ) {

  m_x[0] = x;
  m_x[1] = y;
  m_x[2] = z;

  m_dx[0] = dx;
  m_dx[1] = dy;
  m_dx[2] = dz;

  m_Ntracks = Ntracks;
  m_chi2 = c2;
  m_ndof = dof;

  if ( tracks ) addTracks( *tracks );
}


TIDA::Vertex::Vertex( const TIDA::Vertex& v ) 
    : TObject( v ) {

  for ( int i=3 ; i-- ; ) m_x[i]  = v.m_x[i];
  for ( int i=3 ; i-- ; ) m_dx[i] = v.m_dx[i];
  m_Ntracks = v.m_Ntracks;
  m_chi2 = v.m_chi2;
  m_ndof = v.m_ndof;

  // is this fine?
  m_tracks = v.m_tracks;
  m_ids = v.m_ids;
}


void TIDA::Vertex::addTracks( 
    const std::vector<TIDA::Track*>& tracks ) {

  m_tracks = tracks;

  m_ids.clear();
  m_ids.resize( tracks.size() );
  for ( size_t i=0; i<tracks.size(); i++ ) {
    m_ids[i] = tracks[i]->id();
  }
}


void TIDA::Vertex::selectTracks( 
    const std::vector<TIDA::Track*>& trackCollection,
    const std::vector<unsigned long>& trackIds ) {

  m_ids = trackIds;

  m_tracks.clear();
  selectTracks( trackCollection );   
}


void TIDA::Vertex::selectTracks( 
    const std::vector<TIDA::Track*>& trackCollection ) {

  for ( size_t iid = 0; iid < m_ids.size(); iid++ ) {
    for (size_t itrk = 0; itrk < trackCollection.size(); itrk++) {
      TIDA::Track* trk = trackCollection[itrk];
      if ( trk->id() == m_ids[iid] ) {
        addTrack( trk );
        break;
      }
    }
  }
  m_Ntracks = m_tracks.size();
}

