/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef DETAILEDTRACKTRUTH_P1_H
#define DETAILEDTRACKTRUTH_P1_H

#include "TrkTruthTPCnv/SubDetHitStatistics_p0.h"
#include "TrkTruthTPCnv/TruthTrajectory_p1.h"

#include "CLIDSvc/CLASS_DEF.h"

namespace Trk {
  class DetailedTrackTruth_p1 {
  public:
    SubDetHitStatistics_p0 m_hitsCommon;
    SubDetHitStatistics_p0 m_hitsTrack;
    TruthTrajectory_p1 m_trajectory;
  };
}

CLASS_DEF( Trk::DetailedTrackTruth_p1 , 104029967 , 1 )

#endif/*DETAILEDTRACKTRUTH_P1_H*/
