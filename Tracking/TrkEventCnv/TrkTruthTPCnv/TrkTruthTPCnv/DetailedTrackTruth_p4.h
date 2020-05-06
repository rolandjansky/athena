/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2008
// Olivier.Arnaez@cern.ch, 2015


#ifndef DETAILEDTRACKTRUTH_P4_H
#define DETAILEDTRACKTRUTH_P4_H

#include "TrkTruthTPCnv/SubDetHitStatistics_p0.h"
#include "TrkTruthTPCnv/TruthTrajectory_p3.h"

#include "CLIDSvc/CLASS_DEF.h"

namespace Trk {

  class DetailedTrackTruth_p4 {
  public:
    SubDetHitStatistics_p0 m_hitsCommon;
    SubDetHitStatistics_p0 m_hitsTrack;
    SubDetHitStatistics_p0 m_hitsTruth; //! This is a new member in p3
    TruthTrajectory_p3 m_trajectory;
  };
}

CLASS_DEF( Trk::DetailedTrackTruth_p4 , 104029962 , 1 )

#endif/*DETAILEDTRACKTRUTH_P4_H*/
