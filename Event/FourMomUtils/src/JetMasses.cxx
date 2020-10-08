/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMomUtils/JetMasses.h"

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

#include <cmath>

namespace FourMomUtils {

using std::abs;
using std::exp;

bool
jetMasses( const I4MomIter_t& iBeg, const I4MomIter_t& iEnd,
     double& heavyJetMass, double& lightJetMass,
     CLHEP::Hep3Vector thrust )
{
  if(abs(thrust.mag()-1)>0.01)
    return false;

  // split event into upper and lower hemisphere
  CLHEP::Hep3Vector up(0,0,0);
  CLHEP::Hep3Vector down(0,0,0);

  double Q=0;

  for ( I4MomIter_t itr = iBeg; itr != iEnd; ++itr )
    {
      CLHEP::Hep3Vector c( (*itr)->px(), (*itr)->py(), (*itr)->pz() );
      Q   += sqrt( (*itr)->px()*(*itr)->px()+(*itr)->py()*(*itr)->py() );

      if(c.dot(thrust)>0)
        {
          up += c;
        }else{
          down += c;
        }
    }

  const double inv_Q2 = 1. / (Q*Q);
  lightJetMass=down.mag2()*inv_Q2;
  heavyJetMass=up.mag2()*inv_Q2;
  if ( lightJetMass > heavyJetMass )
    {
      lightJetMass=up.mag2()*inv_Q2;
      heavyJetMass=down.mag2()*inv_Q2;
    }

  return true;
}

} //> end namespace FourMomUtils

#endif
