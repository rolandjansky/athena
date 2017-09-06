/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETPERFORMANCEFUNCTIONS_CXX
#define JETPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

void UpgradePerformanceFunctions::setJetRandomSeed(unsigned seed) {
  m_jetRandom.SetSeed(seed);
}

float UpgradePerformanceFunctions::getJetSmearedEnergy(float ptMeV, float eta, bool useJetArea) {
  if(ptMeV < 5000. || ptMeV > 1500000.){
    std::cout << " getJetSmearedEnergy valid only in range 10-1500 GeV)" << std::endl;
    return 0;
  }
  if(fabs(eta) > 4.5){
    //    std::cout << " getJetSmearedEnergy valid only in eta range (-4.5 - 4.5), current eta: " << eta << std::endl;
    return 0;
  }
  
  double mysigma = getJetEnergyResolution(ptMeV, eta, useJetArea);
  double smearFactor = m_jetRandom.Gaus(1, mysigma);
  return std::max(ptMeV*smearFactor, 1E-08);
}

float UpgradePerformanceFunctions::getJetEnergyResolution(float ptMeV, float eta, bool useJetArea) {
  // This currently returns relative energy resolution.

  //eta binning
  //double Etamin[4]={0,0.8,1.2,2.8};
  //double Etamax[4]={0.8,1.2,2.8,4.5};         
  eta = fabs(eta);
  
  double sigma = 0;
  double N = 0; double S = 0; double C = 0;
  double A = 0; double B = 0;
  
  if( 0 <= eta && eta < 0.8){
    C = 0.055;
    S = 0.74;
    if(!useJetArea){
      A = 3.16;
      B = 0.07;
    }           
    else{
      A = 3.2;
      B = 0.035;
    }
  }
  else if(0.8 <= eta && eta < 1.2){
    C = 0.053;
    S = 0.85;
    if(!useJetArea){
      A = 3.03;
      B = 0.07;
    }
    else{
      A = 2.9;
      B = 0.04;
    }
  }
  else if( 1.2 <= eta && eta < 2.8){
    C = 0.056;
    S = 0.56;
    if(!useJetArea){
      A = 3.32;
      B = 0.08;
    }
    else{
      A = 3.5;
      B = 0.03;
    }
  }
  else if( 2.8 <= eta && eta < 4.5){
    C = 0.054;
    S = 0.83;
    if(!useJetArea){
      A = 2.84;
      B = 0.11;
    }
    else{
      A = 2.9;
      B = 0.06;
    }
  }
  else {std::cout << "getJetEnergyResolution: eta value not allowed: " << eta << std::endl;}
  
  N = A + B*m_avgMu; //same as old f_N function
  sigma = sqrt(N*N / (ptMeV/1000. * ptMeV/1000.) + S*S/(ptMeV/1000.) + C*C); //same as old f_reso
  return sigma;
}

// float UpgradePerformanceFunctions::getTrackJetConfirmEff(float ptGeV, float eta) {
//   if (m_avgMu!=150) {
//     std::cout << "WARNING : track-jet confirmation not implemented for mu!=150 !" << std::endl;
//     return 1e6;
//   }
//   if (fabs(eta)>2.1) {
//     std::cout << "WARNING : track-jet confirmation not implemented for abs(eta)>2.1 !" << std::endl;
//     return 1e6;
//   }

//   if (ptGeV<20.) {
//     std::cout << "WARNING : track-jet confirmation not implemented for pT<20000 MeV !" << std::endl;
//     return 0.;
//   }
//   else if (ptGeV<25.)
//     return 0.75;
//   else if (ptGeV<35.)
//     return 0.80;
//   return 0.95;
//}

float UpgradePerformanceFunctions::getJetPtThresholdForFakeRate(float eta, float fakeRate, bool useTrackConfirmation) {
  // Note output is now in MeV
  if (m_avgMu!=150) {
    std::cout << "WARNING : fake rate parametrization not implemented for mu!=150 !" << std::endl;
    return 1e6;
  }
  
  float a=0, b=0;
  if (fabs(eta)<2.1) {
    if (!useTrackConfirmation) {
      a=36.;
      b=-0.16;
    }
    else {
      a=18.;
      b=-0.18;
    }
  }
  else {
    if (useTrackConfirmation) {
      std::cout << "WARNING : Track Confirmation not available for fake rate parametrization about eta=2.1 !" << std::endl;
      return 1e6;
    }
    if (fabs(eta)<2.8) {
      a=32.;
      b=-0.19;
    }
    else if (fabs(eta)<3.2) {
      a=31.;
      b=-0.19;
    }
    else if (fabs(eta)<4.5) {
      a=19.;
      b=-0.21;
    }
  }
  return (a * pow(fakeRate, b))*1000.;
}

#endif
