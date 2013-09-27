/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef TRACKTRUTHKEY_H
#define TRACKTRUTHKEY_H

#include "TrkTrack/TrackCollection.h"
#include "DataModel/ElementLink.h"

namespace Trk {

  // This class identifies a track in TrackTruthCollection and
  // DetailedTrackTruthCollection.  It uses less memory than
  // ElementLink<TrackCollection> because it does not carry a
  // string-the TrackCollection key.  Instead this is a wrapper around
  // a single integer - the index of the track in a TrackCollection.
  //
  // andrei.gaponenko@cern.ch, 2007

  class TrackTruthKey {
  public:
    // Is it enough to use a 32 bit unsigned instead?
    typedef TrackCollection::size_type size_type;

    size_type index() const { return m_index; }

    TrackTruthKey(size_type i) : m_index(i) {}

    // For compatibility with the existing clients of ElementLink
    // based TrackTruthCollection.  This constuctor may be removed
    // after the clients are migrated to use bare index instead of
    // building ElementLink to do truth look up.
    
    TrackTruthKey(const ElementLink<TrackCollection>& el) : m_index(el.index()) {}

    // Reflex is clueless about handling types that do not have a default constructor
    TrackTruthKey() : m_index(-1U) {}

  private:
    size_type m_index; 
  };

  inline bool operator<(const TrackTruthKey& a, const TrackTruthKey& b) {
    return a.index() < b.index();
  }
  
}

#endif/*TRACKTRUTHKEY_H*/
