/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SITRACK_TRIPLET_MERGING_CUTS_H__
#define __TRIG_L2_SITRACK_TRIPLET_MERGING_CUTS_H__

struct TrigL2SiTrackTripletMergingCuts {
  public:
  TrigL2SiTrackTripletMergingCuts() : 
    m_recoMaxTracks(100) {};

  int  m_recoMaxTracks;
  float m_extensionSolveTripletsDRCut; 
  float m_dynamic_extensionCutSpacePoints; 
  float m_extensionSolveTripletsPhiScale; 
  float m_extensionSolveTripletsEtaScale;
};

#endif
