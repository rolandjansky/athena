/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SITRACK_AMBIGUITY_SOLVING_CUTS_H__
#define __TRIG_L2_SITRACK_AMBIGUITY_SOLVING_CUTS_H__

struct TrigL2SiTrackAmbiguitySolvingCuts {
  bool m_extensionSolveShared;
  bool m_extensionSolveV0Tracks;
  float m_extensionSolveTracksDRCut;
  float m_extensionSolveTracksPhiScale;
  float m_extensionSolveTracksEtaScale;
  int m_dynamic_extensionSpacePoints;
};

#endif
 
