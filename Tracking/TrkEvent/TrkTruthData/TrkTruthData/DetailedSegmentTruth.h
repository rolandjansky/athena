/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef DETAILEDSEGMENTTRUTH_H
#define DETAILEDSEGMENTTRUTH_H

#include "TrkTruthData/SubDetHitStatistics.h"
#include "TrkTruthData/TruthTrajectory.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "CLIDSvc/CLASS_DEF.h"

class TrackTruthCollectionAccessor;

class DetailedSegmentTruth : public DetailedTrackTruth {
public:
  const Amg::Vector3D& truthSegPos() const { return m_pos; }
  const Amg::Vector3D& truthSegDir() const { return m_dir; }

  DetailedSegmentTruth() {} // for POOL
  DetailedSegmentTruth(const TruthTrajectory& traj,
		     const SubDetHitStatistics& hitsCommon,
		     const SubDetHitStatistics& hitsTrack,
		     const SubDetHitStatistics& hitsTruth,
                     const Amg::Vector3D& pos,
                     const Amg::Vector3D& dir)
    : DetailedTrackTruth(traj,hitsCommon,hitsTrack,hitsTruth),
      m_pos(pos),
      m_dir(dir)
  { 
  }

private:
  Amg::Vector3D  m_pos;
  Amg::Vector3D m_dir;
};

CLASS_DEF(DetailedSegmentTruth, 248806722, 1)

#endif/*DETAILEDSEGMENTTRUTH_H*/
