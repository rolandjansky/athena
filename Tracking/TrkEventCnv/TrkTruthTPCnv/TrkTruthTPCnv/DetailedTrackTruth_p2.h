/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2008


#ifndef DETAILEDTRACKTRUTH_P2_H
#define DETAILEDTRACKTRUTH_P2_H

#include "TrkTruthTPCnv/SubDetHitStatistics_p0.h"
#include "TrkTruthTPCnv/TruthTrajectory_p1.h"

#include "CLIDSvc/CLASS_DEF.h"

namespace Trk {

  class DetailedTrackTruth_p2 {
  public:
    SubDetHitStatistics_p0 m_hitsCommon;
    SubDetHitStatistics_p0 m_hitsTrack;
    SubDetHitStatistics_p0 m_hitsTruth; //! This is a new member in p2
    TruthTrajectory_p1 m_trajectory;
  };
}

CLASS_DEF( Trk::DetailedTrackTruth_p2 , 104029964 , 1 )

#endif/*DETAILEDTRACKTRUTH_P2_H*/
