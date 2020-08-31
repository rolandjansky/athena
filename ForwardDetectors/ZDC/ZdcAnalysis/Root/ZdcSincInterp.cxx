/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZdcSincInterp.h"
#include "TMath.h"
#include <cmath>

namespace ZDC
{
  double SincInterp(double* xvec, double* pvec)
  {
    // pvec are the sample values
    double ret = 0;
    double T = pvec[0]; // deltaT
    double t = xvec[0];
    for (int isamp = 0;isamp<7;isamp++)
      {
	double arg = (t - isamp*T)/T;
	if (arg!=0.0)
	  {
	    ret += pvec[isamp+1] * std::sin(TMath::Pi()*arg)/(TMath::Pi()*arg);
	  }
      }
    return ret;
  }
}
