/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  RoIData_Tau.cxx
*/
#include "TriggerMenuNtuple/RoIData_Tau.h"
#include <iostream>
#include <cmath>

using namespace std;


RoIData_Tau::RoIData_Tau(int create) : 
  RoIData(create), 
  L2TauClusterChains(0), L2TauChains(0), EFTauChains(0),
  TauCluster(false), TauClusterEt(0.0), TauClusterEta(0.0), TauClusterPhi(0.0),
  L2Tau(false), L2TauPt(0.0), L2TauEta(0.0), L2TauPhi(0.0),
  EFTau(false), EFTauPt(0.0), EFTauEta(0.0), EFTauPhi(0.0) {
}

RoIData_Tau::~RoIData_Tau() {
}


void RoIData_Tau::setTauCluster(float et, float eta, float phi, std::vector<TrigStatus> status) {
  TauCluster = true;
  TauClusterEt = et;
  TauClusterEta = eta;
  TauClusterPhi = phi;
  L2TauClusterChains = status;
}

void RoIData_Tau::setL2Tau(float pt, float eta, float phi, std::vector<TrigStatus> status) {
  L2Tau = true;
  L2TauPt = pt;
  L2TauEta = eta;
  L2TauPhi = phi;
  L2TauChains = status;
}

void RoIData_Tau::setEFTau(float pt, float eta, float phi, std::vector<TrigStatus> status) {
  EFTau = true;
  EFTauPt = pt;
  EFTauEta = eta;
  EFTauPhi = phi;
  EFTauChains = status;
}


bool RoIData_Tau::l2exist() const { return L2Tau;}
bool RoIData_Tau::efexist() const { return EFTau;}

float RoIData_Tau::l2pt() const { return fabs(L2TauPt)/1000;}
float RoIData_Tau::efpt() const { return fabs(EFTauPt)/1000;}

float RoIData_Tau::l2eta() const { return L2TauEta;}
float RoIData_Tau::efeta() const { return EFTauEta;}

float RoIData_Tau::l2phi() const { return L2TauPhi;}
float RoIData_Tau::efphi() const { return EFTauPhi;}
