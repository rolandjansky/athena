/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef TRACKTRUTHKEY_P0_H
#define TRACKTRUTHKEY_P0_H

#include "TrkTrack/TrackCollection.h"

namespace Trk {

  struct TrackTruthKey_p0 {
    // Is it enough to use a 32 bit unsigned instead?
    typedef TrackCollection::size_type size_type;
    size_type m_index; 

    TrackTruthKey_p0(size_type i) : m_index(i) {}

    // Reflex is clueless about handling types that do not have a default constructor
    TrackTruthKey_p0() : m_index(-1U) {}

  };

  inline bool operator<(const TrackTruthKey_p0& a, const TrackTruthKey_p0& b) {
    return a.m_index < b.m_index;
  }
  
}

CLASS_DEF( Trk::TrackTruthKey_p0 , 252928432 , 1 )

#endif/*TRACKTRUTHKEY_P0_H*/
