/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasUtilities/EtaPhiFilters.h"
#include "G4PrimaryParticle.hh"
#include <utility>
using namespace std;


// See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlasG4EventFilters for more details.
bool EtaPhiFilters::EditParticle(G4PrimaryParticle* p) {
  if (!m_onSwitch) return true;
  const G4ThreeVector& vec(p->GetMomentum());
  const double phi_tmp(vec.phi());
  const double phi(phi_tmp + ((phi_tmp < 0) ? 2*M_PI : 0.0));
  const double eta(vec.eta());
  if (m_verboseLevel > 0) {
    cout << "EtaPhiFilters::EditParticle: particle with eta,phi: " << eta << " " << phi << endl;
    cout << "EtaPhiFilters::EditParticle: number of Eta not-default intervals to check " << m_eLimits.size() << endl;
    cout << "EtaPhiFilters::EditParticle: number of Phi not-default intervals to check " << m_pLimits.size() << endl;
  }

  // First check eta (if no ranges, always accept)
  if (m_eLimits.size()) {
    bool passedEta(false);
    for (size_t i(0); i < m_eLimits.size(); ++i) {
      if (m_verboseLevel > 0) {
        cout << "EtaPhiFilters::EditParticle: check particle with eta " << eta << " vs. limits [" 
	     << m_eLimits[i].first << ", " << m_eLimits[i].second << "]" << endl;
      }
      if (eta > m_eLimits[i].first && eta < m_eLimits[i].second) {
        passedEta = true;
        break;
      }
    }
    if (!passedEta) {
      if (m_verboseLevel > 0) cout << "EtaPhiFilters::EditParticle: particle rejected with eta = " << eta << endl;
      return false;
    }
  }

  // Check phi (default is to always accept)
  if (m_pLimits.size()) {
    bool passedPhi(false);
    for (size_t i(0); i < m_pLimits.size(); ++i) {
      if (m_verboseLevel > 0) {
        cout << "EtaPhiFilters::EditParticle: check particle with phi " << phi << " vs. limits [" 
	     << m_pLimits[i].first << ", " << m_pLimits[i].second << "]" << endl;
      }
      if (phi > m_pLimits[i].first && phi < m_pLimits[i].second) {
        passedPhi = true;
        break;
      }
    }
    if (!passedPhi) {
      if (m_verboseLevel > 0) cout << "EtaPhiFilters::EditParticle: particle rejected with phi = " << phi << endl;
      return false;
    }
  }

  // Success!
  if (m_verboseLevel > 0) {
    cout << "EtaPhiFilters::EditParticle: particle accepted with eta, phi: " << eta << ", " << phi << endl;
  }
  return true;
}
