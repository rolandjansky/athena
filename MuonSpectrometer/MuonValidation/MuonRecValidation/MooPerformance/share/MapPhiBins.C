/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// phiBin.C returns the phi region (A,B1,C,B2) given an input phi and
// eta. This function simply loops over phi at 1 degree intervals and 
// prints out the values where the phi region changes. Eta can be set 
// for barrel or EC 

#include <cmath>

{

  #include "phiBin.C"
  Double_t eta = 0.0;
  Double_t oneDegree = M_PI/180.0;
  Int_t currentBin(-999);
  for (Int_t phi = -180; phi <= 180; phi++) {
    Int_t bin = phiBin((Float_t)phi*oneDegree,eta);
    if ( bin != currentBin ) {
      currentBin = bin;
      if ( bin == 1 || bin == 2 ) {
	printf("bin %i boundary at %f\n",bin,(Float_t)phi*oneDegree);
      }
    }
  }


}
