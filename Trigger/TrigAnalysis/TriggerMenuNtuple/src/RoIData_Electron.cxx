/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  RoIData_Electron.cxx
*/
#include "TriggerMenuNtuple/RoIData_Electron.h"
#include <iostream>

using namespace std;


RoIData_Electron::RoIData_Electron(int create) : 
  RoIData(create), 
  L2CaloChains(0),L2IDChains(0),L2ElectronChains(0),
  EFegammaChains(0), 
  EMCluster(false), EMClusterEt(0.0), EMClusterEta(0.0), EMClusterPhi(0.0),
  EFegamma(false), EFegammaPt(0.0), EFegammaEta(0.0), EFegammaPhi(0.0) {
  L2_E=0.0;
  L2_Et=0.0;
  L2_eta=0.0;
  L2_phi=0.0;

  /*** TrigElectron ***/
  L2_charge=0;
  L2_Ethad1=0.0;
  L2_Eratio=0.0;
  L2_reta=0.0;
  L2_trackclusterdeta=0.0;
  L2_trackclusterdphi=0.0;
  L2_Etoverpt=0.0;
  L2_trackpt=0.0;
  L2_trackalgo=TrigInDetTrack::NULLID;

  EF_E=0.0;
  EF_Et=0.0;
  EF_eta=0.0;
  EF_phi=0.0;
  EF_charge=0;
  EF_Ethad1=0.0;               //EMShower
  EF_Ethad=0.0;
  EF_rphiallcalo=0.0;
  EF_E233=0.0;
  EF_E237=0.0;
  EF_E277=0.0;
  EF_f1=0.0;
  EF_Emins1=0.0;
  EF_fside=0.0;
  EF_Emax2=0.0;
  EF_ws3=0.0;
  EF_stot=0.0;
  EF_weta2=0.0;
}

RoIData_Electron::~RoIData_Electron() {
}

void RoIData_Electron::setEMCluster(float e, float et, float eta, float phi,
				    std::vector<TrigStatus> status){
  EMCluster = true;
  EMClusterEt = et;
  EMClusterEta = eta;
  EMClusterPhi = phi;
  L2CaloChains = status;

  L2_E = e;
  L2_Et = et;
  L2_eta = eta;
  L2_phi = phi;
}

void RoIData_Electron::setL2ID(std::vector<TrigStatus> status){
  L2IDChains = status;
}

void RoIData_Electron::setL2Electron(int charge, float ethad, float eratio , float reta, 
				     float deta, float dphi, float etoverpt, float trackpt,
				     TrigInDetTrack::AlgoId trackalgo, std::vector<TrigStatus> status){
  L2_charge = charge;
  L2_Ethad1 = ethad;
  L2_Eratio = eratio;
  L2_reta = reta;
  L2_trackclusterdeta = deta;
  L2_trackclusterdphi = dphi;
  L2_Etoverpt = etoverpt;
  L2_trackpt = trackpt;
  L2_trackalgo= trackalgo;
  L2ElectronChains = status;
}

void RoIData_Electron::setEFegamma(float pt, float eta, float phi, std::vector<TrigStatus> status,
				   float e, float et, float rphi, float e233, float e237, float e277,
				   float f1, float emins, float fside, float emax, float ws3,
				   float stot, float weta, float ethad, float ethad1, int charge){
  EFegamma = true;
  EFegammaPt = pt;
  EFegammaEta = eta;
  EFegammaPhi = phi;
  EFegammaChains = status;

  EF_E = e;
  EF_Et = et;
  EF_eta = eta;
  EF_phi = phi;
  EF_charge = charge;
  EF_Ethad1 = ethad1;
  EF_Ethad = ethad;
  EF_rphiallcalo = rphi;
  EF_E233 = e233;
  EF_E237 = e237;
  EF_E277 = e277;
  EF_f1 = f1;
  EF_Emins1 = emins;
  EF_fside = fside;
  EF_Emax2 = emax;
  EF_ws3 = ws3;
  EF_stot = stot;
  EF_weta2 = weta;
  EF_charge = charge;
}

bool RoIData_Electron::l2exist() const { return EMCluster;}
bool RoIData_Electron::efexist() const { return EFegamma;}

float RoIData_Electron::l2pt() const { return EMClusterEt/1000;}
float RoIData_Electron::efpt() const { return EFegammaPt/1000;}

float RoIData_Electron::l2eta() const { return EMClusterEta;}
float RoIData_Electron::efeta() const { return EFegammaEta;}

float RoIData_Electron::l2phi() const { return EMClusterPhi;}
float RoIData_Electron::efphi() const { return EFegammaPhi;}

bool RoIData_Electron::L2CaloChainsIsPassed(){return isPassed(getChainToCheck(), L2CaloChains);}
bool RoIData_Electron::L2IDChainsIsPassed(){return isPassed(getChainToCheck(), L2IDChains);}
bool RoIData_Electron::L2ElectronChainsIsPassed(){return isPassed(getChainToCheck(), L2ElectronChains);}
bool RoIData_Electron::EFegammaChainsIsPassed(){return isPassed(getChainToCheck(), EFegammaChains);}

bool RoIData_Electron::L2CaloChainsIsAssociated(){return isAssociated(getChainToCheck(), L2CaloChains);}
bool RoIData_Electron::L2IDChainsIsAssociated(){return isAssociated(getChainToCheck(), L2IDChains);}
bool RoIData_Electron::L2ElectronChainsIsAssociated(){return isAssociated(getChainToCheck(), L2ElectronChains);}
bool RoIData_Electron::EFegammaChainsIsAssociated(){return isAssociated(getChainToCheck(), EFegammaChains);}
