/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SITRACK_TRIPLET_CUTS_H__
#define __TRIG_L2_SITRACK_TRIPLET_CUTS_H__

struct TrigL2SiTrackTripletCuts {
  public:
  TrigL2SiTrackTripletCuts() : 
    m_tripletMakingLimit(100.0) {};

  float m_tripletMakingLimit;
  float m_recoPtMinTripletDiff;
  float m_phiHalfWidth;
  float m_recoD0MaxTripletDiff;
  float m_dynamicalExtensionCutRPhi;
  float m_dynamicalExtensionCutRZPixel;
  float m_dynamicalExtensionCutRZ;
  float m_recoPtMin;
  float m_etaHalfWidth;
  float m_recoD0Max;
  int m_dynamic_extensionCutSpacePoints;
  float m_roiEta;
  float m_roiPhi;
};

#endif
