/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for SubDetHitStatistics.

// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthTPCnv/SubDetHitStatisticsCnv_p0.h"
#include "TrkTruthTPCnv/SubDetHitStatistics_p0.h"
#include "TrkTruthData/SubDetHitStatistics.h"

#include "TrackTruthCollectionAccessor.h"

#include <algorithm>
#include "CxxUtils/copy_bounded.h"

#define ARY_SIZE(x) (sizeof(x)/sizeof(x[0]))


void SubDetHitStatisticsCnv_p0::persToTrans( const Trk::SubDetHitStatistics_p0* pers,
					     SubDetHitStatistics* trans, 
					     MsgStream& /*msg*/ ) 
{
  unsigned char *phits(TrackTruthCollectionAccessor::numPRDs(trans));
  unsigned char* p = 
    CxxUtils::copy_bounded (pers->numPRDs,
                            pers->numPRDs + ARY_SIZE(pers->numPRDs),
                            phits,
                            phits + SubDetHitStatistics::NUM_SUBDETECTORS);
  std::fill (p, phits + SubDetHitStatistics::NUM_SUBDETECTORS, 0);
}

void SubDetHitStatisticsCnv_p0::transToPers( const SubDetHitStatistics* trans, 
					     Trk::SubDetHitStatistics_p0* pers, 
					     MsgStream& /*msg*/ ) 
{
  const unsigned char *phits(TrackTruthCollectionAccessor::numPRDs(trans));
  unsigned char* p = 
    CxxUtils::copy_bounded (phits,
                            phits + SubDetHitStatistics::NUM_SUBDETECTORS,
                            pers->numPRDs,
                            pers->numPRDs + ARY_SIZE(pers->numPRDs));
  std::fill (p, pers->numPRDs + ARY_SIZE(pers->numPRDs), 0);
}
