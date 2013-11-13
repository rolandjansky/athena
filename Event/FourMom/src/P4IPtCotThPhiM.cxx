/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4IPtCotThPhiM.h"

P4IPtCotThPhiM::P4IPtCotThPhiM(const double iPt, const double cotTh, const double phi, const double m):
  m_cotTh(cotTh),m_phi(phi),m_m(m)
{
  //inverse pT is unsigned. I put std::fabs there just for safety
  m_iPt=std::fabs(iPt);
  

 // could enforce phi range convention there
  // const double twopi =2.*M_PI;
  // const double threepi=3.*M_PI;
  // m_phi=fmod(phi+threepi,twopi)-M_PI;
}

const I4MomentumError* P4IPtCotThPhiM::errors() const
{
  return 0;
}
