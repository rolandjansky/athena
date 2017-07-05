/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPERFORMANCEFUNCTIONS_CXX
#define MUONPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

void UpgradePerformanceFunctions::setMuonWorkingPoint(MuonCutLevel cutLevel) {
  m_muonCutLevel = cutLevel;
}

float UpgradePerformanceFunctions::getMuonEfficiency(float ptMeV, float eta) {
  // Returns the muon efficiency for the specified eta region. 
  // For 'tight=true' this corresponds to 0.97 everywhere, except the acceptance gap at eta~0
  // For 'tight=false' (i.e. loose muons) is 0.99 everywhere, except the acceptance gap at eta~0*/
  //M.Corradi: NEW VERSION FROM 30/7/2013
  // using 2012 values from M. Goblirsch

    if (ptMeV < 4000.|| fabs(eta)>4.0) return 0;

    double max_eta =2.7;
    if  (m_layout == gold || m_layout == LoIVF ) max_eta=4.0;
    
    // large eta extension
    if (fabs(eta)>2.7){
        if (m_layout == gold || m_layout == LoIVF )
        {
            if (m_muonCutLevel == looseMuon){ // large-eta extension
                return 0.99;
            } else {
                return 0.95;
            }
        }
        return 0;
    }
    
    if (m_muonCutLevel == looseMuon) {  // loose selection : CB + ST + calo tagged for |eta|<0.1 + SA for 2.5<|eta|<2.7
      return 0.99; 
    } else if (m_muonCutLevel == tightMuon) { // CB "tight" selection
        if (fabs(eta)<0.1) {
            return 0.54;
        } else if (fabs(eta)<max_eta) {
            return 0.97;
        } else {
            return 0;
        }
    }
    std::cout << "getMuonEfficiency: Invalid working point." << std::endl;
    return 0.;
}

float UpgradePerformanceFunctions::getMuonPtResolution(float ptMeV, float eta) {
  // All of the coefficients expect pT in GeV, so we convert the input and output
  float ptGeV = ptMeV/1000.;

  // ID resolution parameters, for use only if ITK resolution not available //
  // double a1_barrel(0.01607);
  // double a2_barrel(0.000307); // unit: 1/GeV
  
  // double a1_endcap(0.030);
  // double a2_endcap(0.000387); // unit: 1/GeV
  
  // double a1, a2;
  
  // MS resolution parameters //

  double b0_barrel(0.24); // unit: GeV
  double b1_barrel(0.02676);
  double b2_barrel(0.00012); // unit: 1/GeV
  
  double b0_endcap(0.0); // unit: GeV
  double b1_endcap(0.0388);
  double b2_endcap(0.00016); // unit: 1/GeV
  
  double b0, b1, b2;

  // resolution components //
  double sigma_ID, sigma_MS, sigma_CB;
    
  ///////////////////////////////////////
  // CALCULATE THE MOMENTUM RESOLUTION //
  ///////////////////////////////////////

  
  sigma_ID = getTrackPtResolution(ptMeV, eta) / 1000.;
  //sigma_ID = 10.;  


  if (fabs(eta)>2.7) { // large eta tagger momentum measurement comes from ITK
      return sigma_ID*1000;
  }
  
  
  // a parameters are for ID, b parameters are for MS
  if (fabs(eta)<1.05) {
    //    a1 = a1_barrel;
    //    a2 = a2_barrel;
    
    b0 = b0_barrel;
    b1 = b1_barrel;
    b2 = b2_barrel;
  } else {
    // a1 = a1_endcap;
    // a2 = a2_endcap;
    
    b0 = b0_endcap;
    b1 = b1_endcap;
    b2 = b2_endcap;
  }

  sigma_MS = ptGeV*sqrt((b0/ptGeV)*(b0/ptGeV)+
			b1*b1+
			(b2*ptGeV)*(b2*ptGeV));

  
  // Combine ID and MS to get combined resolution CB
  
  if (sigma_ID > 0.) {
    sigma_CB = sigma_ID*sigma_MS/sqrt(sigma_ID*sigma_ID+sigma_MS*sigma_MS);
  } else {
    sigma_CB = sigma_MS;
  }
  return sigma_CB*1000.; // back to MeV
}

float  UpgradePerformanceFunctions::getMuonQOverPtResolution(float ptMeV, float eta){
    // This is resolution of q/p_T :
    return getMuonPtResolution(ptMeV, eta)/ptMeV/ptMeV;
}



#endif
