/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUPERFORMANCEFUNCTIONS_CXX
#define TAUPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

void UpgradePerformanceFunctions::setTauRandomSeed(unsigned seed) {
  m_tauRandom.SetSeed(seed);
}

float UpgradePerformanceFunctions::getTauEfficiency(float etMeV, float eta, short prong) {
  // prong variable must be 1 or 3
  float Reco_Pt_cut = 20000.;
  float Reco_eta_cut = 2.5;
  if (etMeV<Reco_Pt_cut || fabs(eta)>Reco_eta_cut){
    std::cout << "WARNING: No Reco SF for these taus - either pt or eta are out of the acceptance !" << std::endl;
    return 0;
  }
  if (prong==1)
    return 0.55;
  else if (prong==3) 
    return 0.50;
  else {
    std::cout << "getTauEfficiency: no efficiency available for prong=" << prong << std::endl;
      return 0;
  }
}

float UpgradePerformanceFunctions::getTauFakeRate(float etMeV, float eta, short prong) {
  float Jet_Pt_cut = 20000.;
  float Jet_eta_cut = 2.5;
  float Fake_SF,Fake_SF_20,Fake_SF_40,Fake_SF_60,Fake_SF_80;
  if (prong==1){
    Fake_SF = 0.05;
    Fake_SF_20 = 0.13;
    Fake_SF_40 = 0.06;
    Fake_SF_60 = 0.03;
    Fake_SF_80 = 0.01;
  } else if (prong==3) {
    Fake_SF = 0.02;
    Fake_SF_20 = 0.47;
    Fake_SF_40 = 0.30;
    Fake_SF_60 = 0.21;
    Fake_SF_80 = 0.09;
  } else {
    std::cout << "getTauFakeRate: no fake rate available for prong=" << prong << std::endl;
    return 0;
  }
  if (etMeV<Jet_Pt_cut || fabs(eta)>Jet_eta_cut) {
    std::cout << "getTauFakeRate: no fake rate available for these jets - either pt or eta are out of the acceptance !" << std::endl;
    return 0;
  }
  else {
    if (etMeV>=20000. && etMeV<40000.)
      return Fake_SF*Fake_SF_20;
    else if (etMeV >= 40000. && etMeV < 60000.)
      return Fake_SF*Fake_SF_40;
    else if (etMeV >= 60000. && etMeV < 80000.) 
      return Fake_SF*Fake_SF_60;
    else if (etMeV >= 80000.)
      return Fake_SF*Fake_SF_80;
  }
  return 0.;
}

float UpgradePerformanceFunctions::getTauEnergyResolution(float eMeV, float eta, short prong) {
  float stocB, stocE;
  // grab parameters for mu=40 and mu=140 points 
  // then interpolate mu between 40 and 140. 
  // do not extrapolate below 40 since we have no studies there
  // linear extrapolate above 140, but only up to 200
  float mu1 = 40.0;
  float mu2 = 140.0;
  float mumin = 40.0;
  float mumax = 200.0;

  float mu = m_avgMu;
  if (mu<mumin){
    std::cout << "WARNING: mu outside range: " << mu << " (mumin: " << mumin << "), choosing " << mumin << std::endl;
    mu = mumin;
  }
  if (mu>mumax){
    std::cout << "WARNING: mu outside range: " << mu << " (mumax: " << mumax << "), choosing " << mumax << std::endl;
    mu = mumax;
  }
  float stocB1, stocB2;
  float stocE1, stocE2;
  if (prong == 1){
    stocB1 = 0.96;
    stocB2 = 1.31;
    stocE1 = 0.04;
    stocE2 = 0.02;
  }
  else if(prong == 3){
    stocB1 = 1.08;
    stocB2 = 1.35;
    stocE1 = 0.04;
    stocE2 = 0.04;
  }
  else{
    std::cout << "getTauEnergyResolution: no energy resolution available for prong=" << prong << " eta=" << eta << std::endl;
    return 0;
  }
  
  //interpolate parameters
  stocB = stocB1 + (mu-mu1)/(mu2-mu1) * (stocB2-stocB1);
  stocE = stocE1 + (mu-mu1)/(mu2-mu1) * (stocE2-stocE1);
  
  double relativeUncertainty = hypot(stocB/sqrt(eMeV/1000.), stocE);
  //  return initE * (m_random3.Gaus(1, (stocB/sqrt(initE/1000.)))) * (m_random3.Gaus(1, stocE)) ;
  return eMeV*relativeUncertainty;
}

float UpgradePerformanceFunctions::getTauSmearedEnergy(float eMeV, float eta, short prong) {
  return (eMeV + m_tauRandom.Gaus(0., getTauEnergyResolution(eMeV, eta, prong)));
}

#endif
