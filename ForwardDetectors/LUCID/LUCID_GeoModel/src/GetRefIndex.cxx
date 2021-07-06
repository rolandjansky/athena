/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <math.h>

#include "LUCID_DetectorFactory.h"
#include "GetRefIndex.h"
#include "GaudiKernel/PhysicalConstants.h"


double GetRefIndexQuartz(double lambda) {
      
  double const SellCoeu[] = {46.41,  228.71, 0.014};
  double const SellCoed[] = {10.666, 18.125, 0.125};
  
  double e = 2.*M_PI*Gaudi::Units::hbarc/(Gaudi::Units::eV*Gaudi::Units::nm)/lambda;
  double r = 1.;
  
  for(int i=0; i<3; i++) r += SellCoeu[i]/(SellCoed[i]*SellCoed[i] - e*e);
  
  return sqrt(r);
}
