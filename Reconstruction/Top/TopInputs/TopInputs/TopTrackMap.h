/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Needed to be able to add TopInputMap<Rec::TrackParticleContainer> to store gate

#ifndef TOP_TRACK_MAP_H
#define TOP_TRACK_MAP_H

#include "Particle/TrackParticleContainer.h"
#include "TopInputs/TopInputMap.h"
#include "SGTools/CLASS_DEF.h"

namespace TopRec {
  typedef  TopInputMap<Rec::TrackParticleContainer> TopTrackMap;
  typedef  ElementLink<Rec::TrackParticleContainer> TopTrackLink;
  typedef  std::pair< TopTrackLink, TopOverlap > TopTrackOverlapPair;
}

CLASS_DEF(TopRec::TopTrackMap, 174028188, 1)

#endif // TOP_TRACK_MAP_H
