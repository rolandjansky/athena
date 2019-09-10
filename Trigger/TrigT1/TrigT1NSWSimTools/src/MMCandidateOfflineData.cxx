/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// Local includes
#include "TrigT1NSWSimTools/MMCandidateOfflineData.h"


namespace NSWL1 {

   MMCandidateOfflineData::MMCandidateOfflineData(int trig_bcid, float delta_theta, float eta_pos, float phi_pos, float eta_sigma_pos, float phi_sigma_pos):
     m_trig_bcid(trig_bcid),
     m_delta_theta(delta_theta),
     m_eta_position(eta_pos),
     m_phi_position(phi_pos),
     m_eta_sigma_position(eta_sigma_pos),
     m_phi_sigma_position(phi_sigma_pos)
   {}

    MMCandidateOfflineData::~MMCandidateOfflineData() {} 

    int   MMCandidateOfflineData::trig_BCID()    const { return m_trig_bcid; }

    float MMCandidateOfflineData::deltaTheta()  const { return m_delta_theta; }
    float MMCandidateOfflineData::eta()         const { return m_eta_position; }
    float MMCandidateOfflineData::etaSigma()    const { return m_phi_position; }
    float MMCandidateOfflineData::phi()         const { return m_eta_sigma_position; }
    float MMCandidateOfflineData::phiSigma()    const { return m_phi_sigma_position; }
}
