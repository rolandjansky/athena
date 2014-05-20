/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPRIM_TRACKSEEDMAP_H
#define TRKPRIM_TRACKSEEDMAP_H

#include <map>
#include "TrkTrack/Track.h"
#include "CLIDSvc/CLASS_DEF.h"

 /** @brief multimap to be used for TrackAmbiguity/Seed validation */
namespace Trk {
  typedef std::multimap<const Trk::Track* , int> TrackSeedMap;
}
CLASS_DEF( Trk::TrackSeedMap , 183190752 , 1 )

#endif
