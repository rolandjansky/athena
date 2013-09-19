/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTruthCreatorTools/TruthMatchTanimoto.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTruthData/SubDetHitStatistics.h"

namespace Trk {

//================================================================
TruthMatchTanimoto::TruthMatchTanimoto(const std::string& type, const std::string& name, const IInterface* parent)
  :  CommonTruthMatchWeights(type,name,parent)
{
  declareInterface<IDetailedTrackTruthSimilarity>(this);
}

//================================================================
double TruthMatchTanimoto::trackTruthSimilarity(const DetailedTrackTruth& dt)
{
  double prd_truth=0, prd_track=0, prd_common=0;
  for(unsigned i=0; i<SubDetHitStatistics::NUM_SUBDETECTORS; i++) {
    prd_common += m_subDetWeights[i] * dt.statsCommon()[SubDetHitStatistics::SubDetType(i)];
    prd_track += m_subDetWeights[i] * dt.statsTrack()[SubDetHitStatistics::SubDetType(i)];
    prd_truth += m_subDetWeights[i] * dt.statsTruth()[SubDetHitStatistics::SubDetType(i)];
  }
 
  double denom = prd_track + prd_truth - prd_common;

  return (denom>0)? prd_common/denom : 0.;
}

//================================================================

} // namespace Trk
