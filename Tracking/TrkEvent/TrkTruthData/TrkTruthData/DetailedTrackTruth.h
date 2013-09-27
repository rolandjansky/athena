/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef DETAILEDTRACKTRUTH_H
#define DETAILEDTRACKTRUTH_H

#include "TrkTruthData/SubDetHitStatistics.h"
#include "TrkTruthData/TruthTrajectory.h"

#include "CLIDSvc/CLASS_DEF.h"

class TrackTruthCollectionAccessor;

class DetailedTrackTruth {
public:
  const TruthTrajectory& trajectory() const { return m_trajectory; }
  const SubDetHitStatistics& statsCommon() const { return m_hitsCommon; }
  const SubDetHitStatistics& statsTrack() const { return m_hitsTrack; }
  const SubDetHitStatistics& statsTruth() const { return m_hitsTruth; }

  DetailedTrackTruth() {} // for POOL
  DetailedTrackTruth(const TruthTrajectory& traj,
		     const SubDetHitStatistics& hitsCommon,
		     const SubDetHitStatistics& hitsTrack,
		     const SubDetHitStatistics& hitsTruth)
    : m_hitsCommon(hitsCommon),
      m_hitsTrack(hitsTrack),
      m_hitsTruth(hitsTruth),
      m_trajectory(traj)
  {}

protected:
  friend class TrackTruthCollectionAccessor; // in TrackTruthTPCnv
  SubDetHitStatistics m_hitsCommon;
  SubDetHitStatistics m_hitsTrack;
  SubDetHitStatistics m_hitsTruth;
  TruthTrajectory m_trajectory;
};

//================================================================
inline std::ostream& operator<<(std::ostream& os, const DetailedTrackTruth& m) {
  return 
    os<<"DetailedTrackTruth(common="<<m.statsCommon()
      <<", ontrack="<<m.statsTrack()
      <<", ontruth="<<m.statsTruth()
      <<", traj="<<m.trajectory()<<")";
}

CLASS_DEF(DetailedTrackTruth, 148183402, 1)

#endif/*DETAILEDTRACKTRUTH_H*/
