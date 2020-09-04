/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMomUtils/FoxWolfram.h"

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS


#include <cmath>

namespace FourMomUtils {

using std::abs;
using std::exp;
using std::cos;

bool
foxWolfram( const I4MomIter_t& iBeg, const I4MomIter_t& iEnd,
      std::vector<double>& H, unsigned int order )
{
  // this is the vector of results
  H.resize(order, 0);
  if(order==0) return true;

  double N0=0;

  // store the first four values of the legendre polynomials in vector,
  //  then use recursive formula to calculate others (just need two previous elements)
  std::vector<double> P(order, 1);

  for ( I4MomIter_t itr_i = iBeg; itr_i != iEnd; ++itr_i )
    {
      CLHEP::Hep3Vector ci( (*itr_i)->px(), (*itr_i)->py(), (*itr_i)->pz() );

      for ( I4MomIter_t itr_j = iBeg; itr_j != iEnd; ++itr_j )
        {
          CLHEP::Hep3Vector cj( (*itr_j)->px(), (*itr_j)->py(), (*itr_j)->pz() );
          double x=cos(ci.angle(cj));

          double P0=1;
          double P1=x;
          double P2=0.5*(3.0*x*x-1);
          double P3=0.5*(5.0*x*x*x-3.0*x);
          double P4=0.125*(35.0*x*x*x*x-30*x*x+3);

          H[0]+=abs(ci.mag())*abs(cj.mag())*P0;
          if(order>=1)
            {
              H[1]+=abs(ci.mag())*abs(cj.mag())*P1;
              P[1]=P1; // never used
            }
          if(order>=2)
            {
              H[2]+=abs(ci.mag())*abs(cj.mag())*P2;
              P[2]=P2; // never used
            }
          if(order>=3)
            {
              H[3]+=abs(ci.mag())*abs(cj.mag())*P3;
              P[3]=P3;
            }
          if(order>=4)
            {
              H[4]+=abs(ci.mag())*abs(cj.mag())*P4;
              P[4]=P4;
            }

          for ( unsigned int loop=5; loop<order; ++loop )
            {
              P[loop] = 1.0/loop*((2.0*loop-1)*x*P[loop-1]-
                                  (loop-1)*P[loop-2]);
              H[loop]=abs(ci.mag())*abs(ci.mag())*P[loop];
            }
        }
      N0+=(*itr_i)->e();
    }

  N0=N0*N0;

  // and normalize
  const double inv_N0 = 1. / N0;
  for ( unsigned int loop=5; loop<order; ++loop ) {
    H[loop] *= inv_N0;
  }

  return true;
}

} //> end namespace FourMomUtils


#endif
