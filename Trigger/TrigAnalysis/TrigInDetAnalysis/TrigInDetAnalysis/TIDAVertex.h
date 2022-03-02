/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDAVertex.h
 **
 **     @author  emil haines, mark sutton
 **     @date    Wed  28 Jul 2021
 **
**     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  TIDA_VERTEX_H
#define  TIDA_VERTEX_H

#include <iostream>
#include <vector>

#include "TrigInDetAnalysis/Track.h"


namespace TIDA {

class Vertex : public TObject {

public:

  Vertex( double    x=0, double  y=0, double  z=0,
          double   dx=0, double dy=0, double dz=0, 
          int Ntracks=0, double c2=0, int   dof=0, 
          const std::vector<TIDA::Track*>* tracks=0 );

  // copy constructor
  Vertex(const TIDA::Vertex& v);

  // destructor
  virtual ~Vertex() {}

  // Tracks passed to addTracks() should belong to the vertex
  void addTracks( const std::vector<TIDA::Track*>& tracks );

  // Select tracks with specific ids from a track collection and add to vertex
  void selectTracks( const std::vector<TIDA::Track*>& trackCollection,
                     const std::vector<unsigned long>& trackIds );

  void selectTracks( const std::vector<TIDA::Track*>& trackCollection );

  // vertex parameter accessors
  const double* position() const { return m_x; }
  double*       position()       { return m_x; }

  double x() const { return m_x[0]; }
  double y() const { return m_x[1]; }
  double z() const { return m_x[2]; }

  const double* error() const { return m_dx; }
  double*       error()       { return m_dx; }

  double dx() const { return m_dx[0]; }
  double dy() const { return m_dx[1]; }
  double dz() const { return m_dx[2]; }

  int Ntracks() const { return m_Ntracks; }

  double chi2() const { return m_chi2; }
  int    ndof() const { return m_ndof; }

  const std::vector<TIDA::Track*>& tracks() const { return m_tracks; }

  const std::vector<unsigned long>& ids() const { return m_ids; }

  const TIDA::Track* operator[] ( const int i ) const {
    if ( m_tracks.size() > 0 ) return m_tracks.at( i );
    return nullptr; }

private:

  // internal track addition method
  void addTrack( TIDA::Track* trk ) { m_tracks.push_back(trk); }

  // vertex parameters
  double m_x[3];
  double m_dx[3];

  int    m_Ntracks;
  double m_chi2;
  int    m_ndof;

  // vertex tracks
  std::vector<TIDA::Track*> m_tracks; //!  
  std::vector<unsigned long> m_ids;

  ClassDef(TIDA::Vertex,3)

};

}

inline std::ostream& operator<<( std::ostream& s, const TIDA::Vertex& t ) { 
  return s <<  "[ x=" << t.position()[0]  << " +- " << t.error()[0]
	   <<  "\ty=" << t.position()[1]  << " +- " << t.error()[1]
	   <<  "\tz=" << t.position()[2]  << " +- " << t.error()[2]
	   << ";\tchi2="   << t.chi2()    << "/" << t.ndof()
	   << "\tNtracks=" << t.Ntracks() << "\t]"; 
}

#endif // TIDA_VERTEX_H