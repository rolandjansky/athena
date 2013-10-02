/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTUTILS_TRACKMATCHINGMULTIMAP_H
#define TRKEVENTUTILS_TRACKMATCHINGMULTIMAP_H

#include <map>
#include "TrkTrack/Track.h"
#include "CLIDSvc/CLASS_DEF.h"

/**This typedef represents a multimap that holds the matching chi2 of two tracks
 from different track collections together with a boolean that indicatges if the 
 matching was succesful**/


namespace Trk {
 
// define output struct of track matching
  struct TrackMatchingProperties {
   double chi2;
   bool matchingSuccesful;
  };
 
  typedef std::multimap <const Trk::Track*, std::pair<const Trk::Track*, Trk::TrackMatchingProperties> > TrackMatchMultiMap;
 }
 
CLASS_DEF(Trk::TrackMatchMultiMap , 112784297, 1 )

#endif
