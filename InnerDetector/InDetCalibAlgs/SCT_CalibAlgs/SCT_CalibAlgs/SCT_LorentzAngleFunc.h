/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//For SCT Lorentz Angle

#include "TMath.h"

Double_t
LA_func(Double_t *x, Double_t *par){
  const Int_t    n     = 1000; // number of integration step                                                                                                                                            
  const Double_t xmin = x[0] - 5.0*par[3];
  const Double_t xmax = x[0] + 5.0*par[3];
  const Double_t h    = (xmax - xmin)/(n - 1);
  const Double_t pi = 3.14159265;
  Double_t sum = 0;
  Double_t x_  = xmin;
  Double_t LA_sum = 0;
  for (Int_t i=1; i<n; i++) {
    LA_sum= par[0]*TMath::Abs(TMath::Tan(x_*pi/180)-TMath::Tan(par[1]*pi/180))+par[2];
    sum  += LA_sum * 0.017455/TMath::Sqrt(2*pi)/(par[3]*pi/180)*TMath::Exp(-(x[0]*pi/180 - x_*pi/180 )*(x[0]*pi/180 - x_*pi/180 )/(2*par[3]*pi/180*par[3]*pi/180))*h;
    x_ += h;
  }
  return   sum;
}


