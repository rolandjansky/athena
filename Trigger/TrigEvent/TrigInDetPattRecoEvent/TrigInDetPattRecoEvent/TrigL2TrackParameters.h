/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_TRACK_PARAMETERS_H__
#define __TRIG_L2_TRACK_PARAMETERS_H__

class TrigL2TrackParameters {

 public:
  TrigL2TrackParameters(float d0, float z0, float phi0, float eta, float pT, float chi2 = 1e8) : 
    m_d0(d0), m_z0(z0), m_phi0(phi0), m_eta(eta), m_pT(pT), m_chi2(chi2) {
  }
  ~TrigL2TrackParameters(){};

  float m_d0, m_z0, m_phi0, m_eta, m_pT, m_chi2;
};



#endif
