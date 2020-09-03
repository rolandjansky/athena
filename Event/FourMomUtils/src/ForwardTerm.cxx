/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "FourMomUtils/ForwardTerm.h"

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS


namespace FourMomUtils {

using std::abs;
using std::exp;

double
forwardTerm( const I4MomIter_t& iBeg,
       const I4MomIter_t& iEnd,
       double central, bool useThreeD )
{
  double Q = 0;
  double suppressed = 0;
  double eta = 0;

  // determine average eta for event
  for ( I4MomIter_t itr = iBeg; itr != iEnd; ++itr )
    {
      if(abs((*itr)->eta())<central)
        {
          // default : standard definition of z component zero
          double z=0;
          if(useThreeD)
            z=(*itr)->pz();

          CLHEP::Hep3Vector c( (*itr)->px(), (*itr)->py(), z );
          Q   += c.mag();
          eta += (*itr)->eta() * c.mag();
        }
    }

  if (Q <= 0)
    return 0;
  const double inv_Q = 1. / Q;

  eta *= inv_Q;

  for ( I4MomIter_t itr = iBeg; itr != iEnd; ++itr )
    {
      if(abs((*itr)->eta())>central)
        {
          // default : standard definition of z component zero
          double z=0;
          if(useThreeD)
            z=(*itr)->pz();

          CLHEP::Hep3Vector c( (*itr)->px(), (*itr)->py(), z );
          suppressed += c.mag() * exp( -abs( eta - (*itr)->eta() ));
        }
    }

  return suppressed * inv_Q;
}

} //> end namespace FourMomUtils


#endif
