/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasUtilities/EtaPhiFilters.h"
#include "G4PrimaryParticle.hh"
#include <utility>
using namespace std;


// See https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlasG4EventFilters for more details.
bool EtaPhiFilters::EditParticle(G4PrimaryParticle* p) {
  if (!onSwitch) return true;
  const G4ThreeVector& vec(p->GetMomentum());
  const double phi_tmp(atan2(vec.y(), vec.x()));
  const double phi(phi_tmp + (phi_tmp < 0) ? 2*M_PI : 0.0);
  const double theta(acos(vec.z()/vec.mag()));
  const double eta(-log(tan(theta/2.)));
  if (verboseLevel > 0) {
    cout << "EtaPhiFilters::EditParticle: particle with eta,phi: " << eta << " " << phi << endl;
    cout << "EtaPhiFilters::EditParticle: number of Eta not-default intervals to check " << eLimits.size() << endl;
    cout << "EtaPhiFilters::EditParticle: number of Phi not-default intervals to check " << pLimits.size() << endl;
  }

  // First check eta (if no ranges, always accept)
  if (eLimits.size()) {
    bool passedEta(false);
    for (size_t i(0); i < eLimits.size(); ++i) {
      if (verboseLevel > 0) {
        cout << "EtaPhiFilters::EditParticle: check particle with eta " << eta << " vs. limits [" 
	     << eLimits[i].first << ", " << eLimits[i].second << "]" << endl;
      }
      if (eta > eLimits[i].first && eta < eLimits[i].second) {
        passedEta = true;
        break;
      }
    }
    if (!passedEta) {
      if (verboseLevel > 0) cout << "EtaPhiFilters::EditParticle: particle rejected with eta = " << eta << endl;
      return false;
    }
  }

  // Check phi (default is to always accept)
  if (pLimits.size()) {
    bool passedPhi(false);
    for (size_t i(0); i < pLimits.size(); ++i) {
      if (verboseLevel > 0) {
        cout << "EtaPhiFilters::EditParticle: check particle with phi " << phi << " vs. limits [" 
	     << pLimits[i].first << ", " << pLimits[i].second << "]" << endl;
      }
      if (phi > pLimits[i].first && phi < pLimits[i].second) {
        passedPhi = true;
        break;
      }
    }
    if (!passedPhi) {
      if (verboseLevel > 0) cout << "EtaPhiFilters::EditParticle: particle rejected with phi = " << phi << endl;
      return false;
    }
  }

  // Success!
  if (verboseLevel > 0) {
    cout << "EtaPhiFilters::EditParticle: particle accepted with eta, phi: " << eta << ", " << phi << endl;
  }
  return true;
}
