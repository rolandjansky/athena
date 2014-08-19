/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SITRACK_SEEDING_CUTS_H__
#define __TRIG_L2_SITRACK_SEEDING_CUTS_H__

struct TrigL2SiTrackSeedingCuts {
  public:
  TrigL2SiTrackSeedingCuts() : 
    m_seedMakingLimit(100.0) {};

  float m_seedMakingLimit;
  float m_recoD0Max;
  float m_recoD0MaxSeedDiff;
  float m_recoPtMin;
  float m_recoPtMinSeedDiff;
  float m_etaHalfWidth;
  float m_roiEta;
  float m_vertexingCutRZ;
  float m_dynamicalSeedingPreCutRPhi;
};

#endif
