/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  EgammaHitsShowerDepth (const CaloRec::Array<2>& sampling_depth,
                              const float& start_crack,
                              const float& end_crack,
                              const float& etamax,
                              MsgStream* log);


  float depth (const float &aeta,
                const xAOD::TrigEMCluster* cluster) const;

  static const CaloSampling::CaloSample m_samps[2][4];

private:
  void barrelCoefs (const float &aeta, float R[4]) const;
  bool endcapCoefs (const float &aeta, float R[4]) const;
  const CaloRec::Array<2>& m_sampling_depth;
  const float& m_start_crack;
  const float& m_end_crack;
  const float& m_etamax;
  MsgStream* m_log;
  

};



#endif
