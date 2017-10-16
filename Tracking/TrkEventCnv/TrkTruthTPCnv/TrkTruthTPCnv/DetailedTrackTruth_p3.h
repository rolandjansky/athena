/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2008
// Olivier.Arnaez@cern.ch, 2015


#ifndef DETAILEDTRACKTRUTH_P3_H
#define DETAILEDTRACKTRUTH_P3_H

#include "TrkTruthTPCnv/SubDetHitStatistics_p0.h"
#include "TrkTruthTPCnv/TruthTrajectory_p2.h"

#include "CLIDSvc/CLASS_DEF.h"

namespace Trk {

  class DetailedTrackTruth_p3 {
  public:
    SubDetHitStatistics_p0 m_hitsCommon;
    SubDetHitStatistics_p0 m_hitsTrack;
    SubDetHitStatistics_p0 m_hitsTruth;
    TruthTrajectory_p2 m_trajectory;
  };
}

CLASS_DEF( Trk::DetailedTrackTruth_p3 , 104129964 , 1 )

#endif/*DETAILEDTRACKTRUTH_P3_H*/
