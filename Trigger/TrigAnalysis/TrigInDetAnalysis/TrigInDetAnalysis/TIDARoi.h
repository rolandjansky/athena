/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDARoi.h
 **
 **     @brief   Roi class, contains RoiDescriptor and also a 
 **              vector of Tracks       
 **
 **     @author  mark sutton
 **     @date    Mon  1 Feb 2010 11:34:52 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TRIGINDETANALYSIS_TRACKROI_H
#define TRIGINDETANALYSIS_TRACKROI_H

#include <iostream>
#include <vector>
#include <string>

#include "TObject.h"

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDAVertex.h"
#include "TrigInDetAnalysis/TrackTrigObject.h"

namespace TIDA { 

class Roi : public TObject {

public:

  Roi();
  Roi(const TIDARoiDescriptor& roi); 

  virtual ~Roi();


  /// access the roi information 
  const TIDARoiDescriptor& roi() const { return m_roi; } 
  TIDARoiDescriptor&       roi()       { return m_roi; } 

  /// accessors for the track vector
  void addTracks( const std::vector<TIDA::Track>& tracks) { 
    m_tracks = tracks;
  }

  void addTracks( const std::vector<TIDA::Track*>& trackp);

  const std::vector<TIDA::Track>& tracks() const { return m_tracks; }
  std::vector<TIDA::Track>&       tracks()       { return m_tracks; }

 /// access the vertices 
  const std::vector<TIDA::Vertex>& vertices() const { return m_vertices; }
  std::vector<TIDA::Vertex>&       vertices()       { return m_vertices; }
  void addVertex( const TIDA::Vertex& v)            {  m_vertices.push_back(v); }
  void addVertices( const std::vector<TIDA::Vertex>& v) { m_vertices = v; }

  /// access the generic objects 
  const std::vector<TrackTrigObject>& objects() const { return m_objects; }
  std::vector<TrackTrigObject>&       objects()       { return m_objects; }
  void addObject( const TrackTrigObject& o) {  m_objects.push_back(o); }
  void addObjects( const std::vector<TrackTrigObject>& ob) {  m_objects.insert( m_objects.end(), ob.begin(), ob.end() ); }

  /// access the user data
  const std::vector<double>& user() const { return m_user; }
  std::vector<double>&       user()       { return m_user; }
  void addUserData( const std::vector<double>& user) { m_user = user; }


  /// clean out everything
  void clear()  { 
    m_tracks.clear(); 
    m_user.clear(); 
    m_objects.clear();
    m_vertices.clear();
  } 

private:

  TIDARoiDescriptor        m_roi; /// roi parameters

  std::vector<TIDA::Track>  m_tracks;   /// tracks in this roi
  std::vector<TIDA::Vertex>               m_vertices; /// vertices


  std::vector<double>                    m_user;    /// user vector
  std::vector<TrackTrigObject>           m_objects; /// objects

  ClassDef(TIDA::Roi,2) 

};

}


inline std::ostream& operator<<( std::ostream& s, const TIDA::Roi& r ) { 
  s << "\tRoi " << r.roi();
  for ( unsigned i=0 ; i<r.tracks().size() ; i++ )   s << "\n\t\t\t" << r.tracks()[i];
  for ( unsigned i=0 ; i<r.user().size() ; i++ )     s << "\n\t\t\t" << r.user()[i];
  for ( unsigned i=0 ; i<r.objects().size() ; i++ )  s << "\n\t\t\t" << r.objects()[i];
  for ( unsigned i=0 ; i<r.vertices().size() ; i++ ) s << "\n\t\t\t" << r.vertices()[i];
  return s;
}


#endif  // TRIGINDETANALYSIS_TRACKROI_H 










