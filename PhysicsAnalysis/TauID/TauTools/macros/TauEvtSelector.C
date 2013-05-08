/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TauEvtSelector.h"
TauEvtSelector::TauEvtSelector(){}
TauEvtSelector::~TauEvtSelector(){}

bool TauEvtSelector::isGoodLBRun00153565(int LBnumber){
  if(LBnumber > 254 && LBnumber < 1149) return true;
  else return false;
}

bool TauEvtSelector::isGoodBCIDRun00153565(int BCID){
  if(BCID==1 || BCID==1786) return true;
  else return false;
}

bool TauEvtSelector::PassedTrigger(bool trigger){ return trigger;}

bool TauEvtSelector::JetsGood(std::vector<int> jet_isGood){
  bool isGood = true; 
 for (unsigned int itr=0; itr<jet_isGood.size(); itr++){
   if(!(jet_isGood[itr]>0)) isGood = false;
  }
 return isGood;
}
bool TauEvtSelector::hasGoodVertex(std::vector<int> vxp_trk_n){
  bool isBad = false;
  for(unsigned int itr=0; itr<vxp_trk_n.size(); itr++){
    if(vxp_trk_n[itr]>3) return true;
  }
  return isBad;
}
bool TauEvtSelector::hasDiTau(std::vector<float> tau_phi, std::vector<float> tau_Et){
  if(tau_phi.size() < 2) return false;
  bool phiPass = false;
  double pi = 3.141592653589793;

  for(unsigned int tau1=0; tau1<tau_phi.size(); tau1++){//loop through tau1
    if(phiPass) break;
    if(tau_Et[tau1] < 15000.) continue;
    for(unsigned int tau2=0; tau2<tau_phi.size(); tau2++){//loop through tau2
      if(tau_Et[tau2] < 15000.) continue;
      double dPhi = tau_phi[tau1]-tau_phi[tau2];
      double deltaPhi = fmod(dPhi+3*pi,2*pi)-pi;
      if(fabs(deltaPhi)>2.7){//have diTau
	phiPass = true;
	break;
      }//have diTau
    }//loop through tau2
  }//loop through tau1
  return phiPass;
}
