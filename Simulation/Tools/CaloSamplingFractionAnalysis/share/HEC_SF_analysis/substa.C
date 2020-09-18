/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// =====================================================================
// 
//  Determine mean value, error of the mean and RMS 
//  for the given set of values
//
//  Inputs     : arr    - array with data
//               narr   - dimension of the array
//
//  Outputs    : ave  - mean value
//               err  - error of the mean
//               rms  - RMS
//              
// =====================================================================

#include "TMath.h"

void substa (
     Double_t *arr, 
     Int_t     narr, 
     Double_t &ave, 
     Double_t &err, 
     Double_t &rms )
{
//
   Int_t i;
//   
   Double_t s0=0, s1=0, s2=0; 
//
/* ------------------------------------------------------------------ */
//
   ave=0; err=0; rms=0;
//   
   if (narr < 1) {
      cout << " *** substa:  Bad narr = " << narr << endl;
      return;
   }
//
//
   for (i=0; i<narr; i++) {
      s0 += 1.0;
      s1 += arr[i];
      s2 += arr[i]*arr[i];
    }
//
    if (s0 == 0) {
      cout << " *** substa:  Bad s0 = " << s0 << endl;
      return;
   }
//
   s1 = s1/s0;
//   
   s2 = s2/s0 - s1*s1;
   if (s2 < 0) {
      cout << " *** substa:  Warning s2 = " << s2 << endl;
      if (TMath::Abs(s2/s1) < 1.e-07)
         s2 = 0;
      else {
         cout << " *** substa:  Bad s2 = " << s2 << endl;
	 exit (-2);
      }	 
   }
//
   ave = s1;
   rms = TMath::Sqrt(s2);
   err = rms / TMath::Sqrt(s0);
//
//
   return;
}
// =====================================================================
