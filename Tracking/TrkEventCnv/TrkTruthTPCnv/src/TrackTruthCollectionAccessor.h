/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef TRACKTRUTHCOLLECTIONACCESSOR_H
#define TRACKTRUTHCOLLECTIONACCESSOR_H

#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTruthData/SubDetHitStatistics.h"
#include "TrkTruthData/TruthTrajectory.h"

// A class to access private data members of TrackTruthCollection from the convertors
class TrackTruthCollectionAccessor {
public:

  // simple truth 

  static DataLink<TrackCollection>& trackCollectionLink(TrackTruthCollection *truthColl) {
    return truthColl->m_trackCollection;
  }
  static const DataLink<TrackCollection>& trackCollectionLink(const TrackTruthCollection *truthColl) {
    return truthColl->m_trackCollection;
  }


  // detailed truth collection

  static DataLink<TrackCollection>& trackCollectionLink(DetailedTrackTruthCollection *truthColl) {
    return truthColl->m_trackCollection;
  }

  static const DataLink<TrackCollection>& trackCollectionLink(const DetailedTrackTruthCollection *truthColl) {
    return truthColl->m_trackCollection;
  }


  // detailed truth item

  static SubDetHitStatistics& statsCommon(DetailedTrackTruth *dt) { return dt->m_hitsCommon; }
  static SubDetHitStatistics& statsTrack(DetailedTrackTruth *dt) { return dt->m_hitsTrack; }
  static SubDetHitStatistics& statsTruth(DetailedTrackTruth *dt) { return dt->m_hitsTruth; }
  static TruthTrajectory& trajectory(DetailedTrackTruth *dt) { return dt->m_trajectory; }

  // SubDetHitStatistics
  static unsigned char* numPRDs(SubDetHitStatistics *stat) { return stat->m_numPRDs; }
  static const unsigned char* numPRDs(const SubDetHitStatistics *stat) { return stat->m_numPRDs; }

};

#endif/*TRACKTRUTHCOLLECTIONACCESSOR_H*/
