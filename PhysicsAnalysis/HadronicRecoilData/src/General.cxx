/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HadronicRecoilData/General.h"
#include <TMath.h>

namespace HadronicRecoil {
  
  //______________________________________________
  double CorrectPhi( double phi )
  {
    while( phi < -TMath::Pi() ){
      phi+= 2*TMath::Pi();
    }
    while( phi >  TMath::Pi() ){
      phi-= 2*TMath::Pi();
    } 
    return phi;
  }

  //______________________________________________
  double DeltaPhi(double phi1, double phi2)
  {
    double Dphi = phi2 - phi1;
    
    return CorrectPhi( Dphi);
  }

  //______________________________________________
  double DeltaR(double eta1, double phi1, double eta2, double phi2)
  {
    double Dphi = DeltaPhi(phi2, phi1);
    
    return TMath::Sqrt( TMath::Power(eta2-eta1,2) + TMath::Power(Dphi,2) );
  }
}
