/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4EEtaPhiM.h"

P4EEtaPhiM::P4EEtaPhiM(const double e, const double eta, const double phi, const double m):
  m_e(e),m_eta(eta),m_phi(phi),m_m(m)
{ // could enforce phi range convention there
  // const double twopi =2.*M_PI;
  // const double threepi=3.*M_PI;
  // m_phi=fmod(phi+threepi,twopi)-M_PI;
}

const I4MomentumError* P4EEtaPhiM::errors() const
{
  return 0;
}
