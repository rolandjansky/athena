/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMomUtils/JetMasses.h"

#include <cmath>

namespace FourMomUtils {

using std::abs;
using std::exp;

bool
jetMasses( const I4MomIter_t iBeg, const I4MomIter_t iEnd,
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
  
  lightJetMass=down.mag2()/Q/Q;
  heavyJetMass=up.mag2()/Q/Q;
  if ( lightJetMass > heavyJetMass )
    {
      lightJetMass=up.mag2()/Q/Q;
      heavyJetMass=down.mag2()/Q/Q;
    }
  
  return true;
}

} //> end namespace FourMomUtils
