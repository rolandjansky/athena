/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  RoIData_Jet.cxx
*/
#include "TriggerMenuNtuple/RoIData_Jet.h"
#include <cmath>
#include <iostream>

using namespace std;


RoIData_Jet::RoIData_Jet(int create) : 
  RoIData(create), 
  L2JetChains(0), EFJetChains(0),
  L2Jet(false), L2JetEt(0.0), L2JetEta(0.0), L2JetPhi(0.0),
  EFJet(false), EFJetEt(0.0), EFJetEta(0.0), EFJetPhi(0.0) {
}

RoIData_Jet::~RoIData_Jet() {
}

bool RoIData_Jet::isAssociatedL2(const std::string& chain_name) const {
  return ::isPassed(chain_name, L2JetChains);
}

bool RoIData_Jet::isAssociatedL2() const {
  return isAssociatedL2(getChainToCheck());
}

bool RoIData_Jet::isAssociatedEF(const std::string& chain_name) const {
  return ::isPassed(chain_name, EFJetChains);
}

bool RoIData_Jet::isAssociatedEF() const {
  return isAssociatedEF(getChainToCheck());
}

void RoIData_Jet::setL2Jet(float et, float eta, float phi, std::vector<TrigStatus> status){
  L2Jet = true;
  L2JetEt = et;
  L2JetEta = eta;
  L2JetPhi = phi; 
  L2JetChains = status;
}

void RoIData_Jet::setEFJet(float et, float eta, float phi, std::vector<TrigStatus> status){
  EFJet = true;
  EFJetEt = et;
  EFJetEta = eta;
  EFJetPhi = phi;
  EFJetChains = status;
}

bool RoIData_Jet::l2exist() const { return L2Jet;}
bool RoIData_Jet::efexist() const { return EFJet;}

float RoIData_Jet::l2pt() const { return fabs(L2JetEt)/1000;}
float RoIData_Jet::efpt() const { return EFJetEt/1000;}

float RoIData_Jet::l2eta() const { return L2JetEta;}
float RoIData_Jet::efeta() const { return EFJetEta;}

float RoIData_Jet::l2phi() const { return L2JetPhi;}
float RoIData_Jet::efphi() const { return EFJetPhi;}
