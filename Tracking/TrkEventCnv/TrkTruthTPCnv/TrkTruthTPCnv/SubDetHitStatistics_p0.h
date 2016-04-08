/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef SUBDETHITSTATISTICS_P0_H
#define SUBDETHITSTATISTICS_P0_H

#include "CLIDSvc/CLASS_DEF.h"
#include "TrkTruthData/SubDetHitStatistics.h"

namespace Trk {
  class SubDetHitStatistics_p0 {
  public:
    //enum SubDetType { Pixel=0, SCT, TRT, MDT, RPC, TGC, CSC, NUM_SUBDETECTORS };
    unsigned char numPRDs[SubDetHitStatistics::NUM_SUBDETECTORS];
    // default ctr leaves memory uninitialized, which is fine.
  };
}

CLASS_DEF( Trk::SubDetHitStatistics_p0 , 234288078 , 1 )

#endif/*SUBDETHITSTATISTICS_P0_H*/
