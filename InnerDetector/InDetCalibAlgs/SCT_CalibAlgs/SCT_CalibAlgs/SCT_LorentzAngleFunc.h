/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//For SCT Lorentz Angle

#include <cmath>
#include "RtypesCore.h" // for Double_t etc.

Double_t
LA_func(Double_t* x, Double_t* par) {
  const Int_t n{1000}; // number of integration step
  const Double_t xmin{x[0] - 5.0*par[3]};
  const Double_t xmax{x[0] + 5.0*par[3]};
  const Double_t h{(xmax - xmin)/(n - 1)};
  const double piOver180 = M_PI/180.;
  const double piNorm = 1./std::sqrt(2.*M_PI);
  Double_t sum{0.};
  Double_t x_{xmin};
  Double_t LA_sum{0.};
  for (Int_t i{1}; i<n; i++) {
    LA_sum = par[0]*std::abs(std::tan(x_*piOver180)-std::tan(par[1]*piOver180))+par[2];
    sum += LA_sum * 0.017455 * piNorm/(par[3]*piOver180)
      * std::exp(-(x[0]*piOver180 - x_*piOver180 )*(x[0]*piOver180 - x_*piOver180 )/(2*par[3]*piOver180*par[3]*piOver180)) * h;
    x_ += h;
  }
  return sum;
}
