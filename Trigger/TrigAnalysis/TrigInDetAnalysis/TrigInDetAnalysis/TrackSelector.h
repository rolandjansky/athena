/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrackSelector.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:37 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/



#ifndef TRACKSELECTOR_H
#define TRACKSELECTOR_H

#include <string>
#include <vector>
#include <algorithm>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/Track.h"


class TrackSelector {
  
public:
  
  TrackSelector( TrackFilter* selector=0 ) :  m_selector(selector) {  } 
  virtual ~TrackSelector() {}

  // add a track, do the selection while adding?
  // or later with a method which loops through them all? 
  virtual bool addTrack(TIDA::Track* t, bool (*f)(const TIDA::Track*)=0 ) {
    //std::cout << "addtrack()  before f: t  " << *t << " " << size() << "\t f  " << f << std::endl;
    if ( f==0 ) { 
      if ( m_selector && m_selector->select(t) )  { m_tracks.push_back(t); return true; }
      else                                      { cleanup(t); }
    }
    else { 
      if ( f(t) )  { m_tracks.push_back(t); return true; } 
      else         { cleanup(t); } 
    }
    return false;
  }

  virtual void addTracks(std::vector<TIDA::Track*>& t, bool (*f)(const TIDA::Track*)=0 ) {
    for ( size_t i=0 ; i<t.size() ; i++ ) addTrack( t[i], f ); 
  }  
  
  // get the selected tracks   
  const std::vector<TIDA::Track*>& tracks() const { return m_tracks; }
  
  std::vector<TIDA::Track*> tracks( TrackFilter* selector ) const {
    if ( selector==0 ) return m_tracks; 
    std::vector<TIDA::Track*> t;
    for ( int i=m_tracks.size() ; i-- ; ) if ( selector->select(m_tracks[i]) ) t.push_back(m_tracks[i]);
    return t;
  }

  // how many tracks in this selection
  unsigned size() const { return m_tracks.size(); } 

  // clear tracks for this roi/event etc 
  //  void clear() { for ( int i=m_tracks.size() ; i-- ; ) delete m_tracks[i]; m_tracks.clear(); } 
  virtual void clear() { m_tracks.clear(); } 

  void delete_track( TIDA::Track* t ) { 
    m_tracks.erase( std::remove( m_tracks.begin(), m_tracks.end(), t ), m_tracks.end() );
  }
  
protected:

  // can be used to delete tracks if they are not added to the selector to avoid memory 
  // leaks if the TrackSelector is managing the tracks itself. 
  virtual void cleanup(TIDA::Track* ) { } 
  // virtual void cleanup(TIDA::Track* t) { delete t; } 
  
protected:
  
  std::vector<TIDA::Track*> m_tracks;
 
  // selection function
  TrackFilter*  m_selector;
 
};



#endif // TRACKSELECTOR_H
