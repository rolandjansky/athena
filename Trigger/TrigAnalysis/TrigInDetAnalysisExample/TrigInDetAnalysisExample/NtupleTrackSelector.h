// emacs: this is -*- c++ -*-

#ifndef __NTUPLETRACKSELECTOR_H
#define __NTUPLETRACKSELECTOR_H

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackSelector.h"
#include "TrigInDetAnalysis/TrackFilter.h"


class NtupleTrackSelector : public TrackSelector { 

public:

  NtupleTrackSelector( TrackFilter* selector=0 ) : TrackSelector(selector) {  } 

  /// add single Track
  void selectTrack( TrigInDetAnalysis::Track* track ) {     
    // do the track extraction stuff here....
    if ( track ) { 
      TrigInDetAnalysis::Track* t = new TrigInDetAnalysis::Track( *track );
      if ( !addTrack( t ) ) delete t;;
      // addTrack( track );
    }
  }


  /// add a Track
  void selectTrack( const TrigInDetAnalysis::Track& track ) {
    // do the track extraction stuff here....
    TrigInDetAnalysis::Track* t = new TrigInDetAnalysis::Track( track );
    if ( !addTrack( t ) ) delete t;
    //  addTrack( &track );
  }

  /// extract all the tracks from a vector of Tracks
  void selectTracks( const std::vector<TrigInDetAnalysis::Track>& tracks ) {     
    // do the track extraction stuff here....
    std::vector<TrigInDetAnalysis::Track>::const_iterator trackitr = tracks.begin();
    std::vector<TrigInDetAnalysis::Track>::const_iterator trackend = tracks.end();
    while ( trackitr!=trackend ) { 
      selectTrack( *trackitr );
      trackitr++;
    }
  }

  /// extract all the tracks from a vector of Tracks pointers
  void selectTracks( const std::vector<TrigInDetAnalysis::Track*>& tracks ) {     
    // do the track extraction stuff here....
    std::vector<TrigInDetAnalysis::Track*>::const_iterator trackitr = tracks.begin();
    std::vector<TrigInDetAnalysis::Track*>::const_iterator trackend = tracks.end();
    while ( trackitr!=trackend ) { 
      //      selectTrack( *(*trackitr) );
      selectTrack( *trackitr );
      trackitr++;
    }
  }

  /// the ntple selector manages the tracks itself, so we have to do an explicit 
  /// delete or each one to prevent memory leaking
  virtual void clear() {  
    for ( int i=mtracks.size() ; i-- ; ) delete mtracks[i]; 
    mtracks.clear(); 
  }

};




#endif // __NTUPLETRACKSELECTOR_H
