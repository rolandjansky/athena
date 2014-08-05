// emacs: this is -*- c++ -*-
//
//   @file    TrackRoi.h        
//
//            Roi class, contains RoiDescriptor and also a 
//            vector of Tracks       
// 
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrackRoi.h, v0.0   Mon  1 Feb 2010 11:34:52 GMT sutt $


#ifndef __TRACKROI_H
#define __TRACKROI_H

#include <iostream>
#include <vector>
#include <string>

#include "TObject.h"

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackVertex.h"
#include "TrigInDetAnalysis/TrackTrigObject.h"


class TrackRoi : public TObject {

public:

  TrackRoi();
  TrackRoi(const TIDARoiDescriptor& roi); 

  virtual ~TrackRoi();


  /// access the roi information 
  const TIDARoiDescriptor& roi() const { return m_roi; } 
  TIDARoiDescriptor&       roi()       { return m_roi; } 

  /// accessors for the track vector
  void addTracks( const std::vector<TrigInDetAnalysis::Track>& tracks) { 
    m_tracks = tracks;
  }

  void addTracks( const std::vector<TrigInDetAnalysis::Track*>& trackp);

  const std::vector<TrigInDetAnalysis::Track>& tracks() const { return m_tracks; }
  std::vector<TrigInDetAnalysis::Track>        tracks()       { return m_tracks; }

 /// access the vertices 
  const std::vector<TrackVertex>& vertices() const { return m_vertices; }
  std::vector<TrackVertex>        vertices()       { return m_vertices; }
  void addVertex( const TrackVertex& v)            {  m_vertices.push_back(v); }
  void addVertices( const std::vector<TrackVertex>& v) { m_vertices = v; }

  /// access the generic objects 
  const std::vector<TrackTrigObject>& objects() const { return m_objects; }
  std::vector<TrackTrigObject>        objects()       { return m_objects; }
  void addObject( const TrackTrigObject& o) {  m_objects.push_back(o); }

  /// access the user data
  const std::vector<double>& user() const { return m_user; }
  std::vector<double>        user()       { return m_user; }
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

  std::vector<TrigInDetAnalysis::Track>  m_tracks;   /// tracks in this roi
  std::vector<TrackVertex>               m_vertices; /// vertices


  std::vector<double>                    m_user;    /// user vector
  std::vector<TrackTrigObject>           m_objects; /// objects

  ClassDef(TrackRoi,2) 

};

//}


inline std::ostream& operator<<( std::ostream& s, const TrackRoi& r ) { 
  s << "\tRoi " << r.roi();
  for ( unsigned i=0 ; i<r.tracks().size() ; i++ )   s << "\n\t\t\t" << r.tracks()[i];
  for ( unsigned i=0 ; i<r.user().size() ; i++ )     s << "\n\t\t\t" << r.user()[i];
  for ( unsigned i=0 ; i<r.objects().size() ; i++ )  s << "\n\t\t\t" << r.objects()[i];
  for ( unsigned i=0 ; i<r.vertices().size() ; i++ ) s << "\n\t\t\t" << r.vertices()[i];
  return s;
}


#endif  // __TRACKROI_H 










