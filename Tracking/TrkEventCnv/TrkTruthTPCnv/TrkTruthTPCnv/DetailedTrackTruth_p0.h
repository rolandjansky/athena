/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef DETAILEDTRACKTRUTH_P0_H
#define DETAILEDTRACKTRUTH_P0_H

#include "TrkTruthTPCnv/SubDetHitStatistics_p0.h"
#include "TrkTruthTPCnv/TruthTrajectory_p0.h"

#include "CLIDSvc/CLASS_DEF.h"

namespace Trk {
  struct DetailedTrackTruth_p0 {
    SubDetHitStatistics_p0 m_hitsCommon;
    SubDetHitStatistics_p0 m_hitsTrack;
    TruthTrajectory_p0 m_trajectory;
  };
}

CLASS_DEF( Trk::DetailedTrackTruth_p0 , 57642330 , 1 )

#endif/*DETAILEDTRACKTRUTH_P0_H*/
