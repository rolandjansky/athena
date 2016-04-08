/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBTrack
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTRACK_P1_H
#define TBTRACK_P1_H


class TBTrack_p1
{

public:

  // number of hits used for reconstruction
  int m_hitNumberU, m_hitNumberV;
  
  // Residuals between detector hit and fitted hit (in detector local coordonates)
  std::vector<double> m_residualu, m_residualv;

  // track parameters
  double m_chi2, m_chi2u, m_chi2v;
  double m_angle;             // angle between track and Z TestBeam axe
  double m_uslope,m_vslope;   // slopes with respect to Z TestBeam axe
  double m_uintercept, m_vintercept;
  
  // extrapoled hit position at cryostat (reconstruction coordonates);
  double m_cryou,m_cryov,m_cryow;    
};


#endif
