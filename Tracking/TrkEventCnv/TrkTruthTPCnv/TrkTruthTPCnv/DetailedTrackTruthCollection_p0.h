/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef DETAILEDTRACKTRUTHCOLLECTION_P0_H
#define DETAILEDTRACKTRUTHCOLLECTION_P0_H

#include "TrkTruthTPCnv/TrackTruthKey_p0.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p0.h"
//#include "TrkTrack/TrackCollection.h"
#include "DataModel/DataLink.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <map>

namespace Trk {
  class DetailedTrackTruthCollection_p0 : public std::multimap<Trk::TrackTruthKey_p0, Trk::DetailedTrackTruth_p0> 
  {
  public:
    // DetailedTrackTruthCollection_p0 is a copy of DetailedTrackTruthCollection before T/P separation
    // that used DataLink, not DataLink_pN
    DataLink<TrackCollection> m_trackCollection;
  };
}
  
CLASS_DEF( Trk::DetailedTrackTruthCollection_p0 , 1281678908 , 1 )

#endif/*DETAILEDTRACKTRUTHCOLLECTION_P0_H*/
