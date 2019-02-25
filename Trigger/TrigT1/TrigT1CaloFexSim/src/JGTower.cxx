/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloFexSim/JGTower.h"
#include "TMath.h"

// Default constructor, should never be used.
// This implementation exists only to print the warning and avoid undefined behavior.
JGTower::JGTower(){
  m_eta = -999;
  m_deta= -999;
  m_phi = -999;
  m_dphi= -999;
}

// Constructor
JGTower::JGTower(float eta, float deta, float phi, float dphi){
  m_eta = eta;
  m_deta = deta;
  m_phi = phi;
  m_dphi = dphi;
}

//Destructor
JGTower::~JGTower(){}

//Tools
bool inBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi){

  bool inEtaBox = fabs(eta1-eta2)<deta;
  float deltaPhi= fabs(phi1-phi2);
  if(deltaPhi>TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;

  bool inPhiBox = deltaPhi<dphi;

  return inEtaBox&&inPhiBox;
}

bool withinRadius(float eta1, float eta2, float phi1, float phi2, float dR, bool acceptEqual){

  float dPhi = deltaPhi(phi1, phi2);
  float dEta = eta1=eta2;
  float dR2 = dPhi*dPhi + dEta*dEta;
  if(acceptEqual)
    return dR2 <= dR*dR;
  else
    return dR2 < dR*dR;
}

float deltaPhi(float phi1,float phi2){

  float dphi = fabs(phi1-phi2);
  if(dphi>TMath::Pi()) dphi = TMath::Pi()*2 - dphi;
  return dphi;
}
