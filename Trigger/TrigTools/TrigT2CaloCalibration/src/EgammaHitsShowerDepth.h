/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOCALIBRATION_EGAMMAHITSSHOWERDEPTH
#define TRIGT2CALOCALIBRATION_EGAMMAHITSSHOWERDEPTH

#include "GaudiKernel/MsgStream.h"

#include "CaloConditions/Array.h"
#include "CaloGeoHelpers/CaloSampling.h"

// CalibHits auxiliar class:
class EgammaHitsShowerDepth
{
public:
  float depth (const float &aeta,
               const float start_crack,
               const float end_crack,
               const CxxUtils::Array<2>& sampling_depth,
               const float etamax,
               const xAOD::TrigEMCluster* cluster) const;

  static const CaloSampling::CaloSample m_samps[2][4];

private:
  void barrelCoefs (const float &aeta, float R[4]) const;
  bool endcapCoefs (const float &aeta,
                    const CxxUtils::Array<2>& sampling_depth,
                    const float etamax,
                    float R[4]) const;
};



#endif
