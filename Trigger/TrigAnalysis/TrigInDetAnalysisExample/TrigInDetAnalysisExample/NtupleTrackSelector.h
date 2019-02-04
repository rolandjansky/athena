/* emacs: this is -*- c++ -*- */
/**
 **     @file    NtupleTrackSelector.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:38 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAEXAMPLE_NTUPLETRACKSELECTOR_H
#define TIDAEXAMPLE_NTUPLETRACKSELECTOR_H

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackSelector.h"
#include "TrigInDetAnalysis/TrackFilter.h"


class NtupleTrackSelector : public TrackSelector { 

public:

  NtupleTrackSelector( TrackFilter* selector=0 ) : TrackSelector(selector) {  } 

  /// add single Track
  void selectTrack( TIDA::Track* track ) {     
    // do the track extraction stuff here....
    if ( track ) { 
      TIDA::Track* t = new TIDA::Track( *track );
      if ( !addTrack( t ) ) delete t;
      // addTrack( track );
    }
  }


  /// add a Track
  void selectTrack( const TIDA::Track& track ) {
    // do the track extraction stuff here....
    TIDA::Track* t = new TIDA::Track( track );
    if ( !addTrack( t ) ) delete t;
    //  addTrack( &track );
  }

  /// extract all the tracks from a vector of Tracks
  void selectTracks( const std::vector<TIDA::Track>& tracks ) {     
    // do the track extraction stuff here....
    std::vector<TIDA::Track>::const_iterator trackitr = tracks.begin();
    std::vector<TIDA::Track>::const_iterator trackend = tracks.end();
    while ( trackitr!=trackend ) { 
      selectTrack( *trackitr );
      trackitr++;
    }
  }

  /// extract all the tracks from a vector of Tracks pointers
  void selectTracks( const std::vector<TIDA::Track*>& tracks ) {     
    // do the track extraction stuff here....
    std::vector<TIDA::Track*>::const_iterator trackitr = tracks.begin();
    std::vector<TIDA::Track*>::const_iterator trackend = tracks.end();
    while ( trackitr!=trackend ) { 
      //      selectTrack( *(*trackitr) );
      selectTrack( *trackitr );
      trackitr++;
    }
  }

  /// the ntple selector manages the tracks itself, so we have to do an explicit 
  /// delete for each one to prevent memory leaking
  virtual void clear() {  
    for ( int i=m_tracks.size() ; i-- ; ) delete m_tracks[i]; 
    m_tracks.clear(); 
  }

};




#endif // TIDAEXAMPLE_NTUPLETRACKSELECTOR_H
