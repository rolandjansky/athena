/*

 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

Rho.h: Calculates energy density rho to subtract as pileup
Author: Ava Myers (amyers@cern.ch)
Methods:
Rho_bar(): Calculates rho as the average tower energy density in the barrel region of the calorimeter, to be scaled with area in more forward regions. An upper threshold of 3 GeV is applied to eliminate bias from hard scatter events

Rho_med(): Calculates rho as the median tower energy density in the barrel, again to be scaled with area

 */

#ifndef TRIGT1CALOFEXSIM_RHO_H
#define TRIGT1CALOFEXSIM_RHO_H

/*
 * Class   : Rho
 *
 * Author  : Myers, Ava (amyers@cern.ch)
 *
 *Calculates energy density rho either as a median or an average to subract as pileup
 */

#include "CaloIdentifier/GTower_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/JGTower.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"
#include <algorithm>
#include <vector>
#include "TMath.h"

/**
 *@brief Calculates rho as the average tower energy density in the barrel region of the calorimeter, to be scaled with are in more forward regions. An upper threshold of 10 GeV is applied to eliminate bias from hard scatter events
 *@return @c float
 */
float Rho_bar(const xAOD::JGTowerContainer* towers, const bool useNegTowers){

  float rho = 0;
  float et_max = 10*Gaudi::Units::GeV; //an upper threshold such that the average rho is not biased by hard scatter events                                                                                                                     
  const unsigned int size = towers->size();
 int length = 0;

 for(unsigned int i = 0; i < size; i++){
   const xAOD::JGTower* tower = towers->at(i);
   float eta = TMath::Abs(tower->eta());
   float Et = tower->et();

   if(!useNegTowers && Et < 0) continue;

   if(eta < 2.4){
     if(Et < et_max){
       rho += Et;
       length++;
     }
   }
 }
 float rho_bar = rho/length;

 return rho_bar;
}

float Rho_bar(std::vector<const xAOD::JGTower*> towers, const bool useNegTowers){

  float rho = 0;
  float et_max = 10*Gaudi::Units::GeV; //an upper threshold such that the average rho is not biased by hard scatter events               

  const unsigned int size = towers.size();
  int length = 0;

  for(unsigned int i = 0; i < size; i++){
    const xAOD::JGTower* tower = towers[i];
    float eta = TMath::Abs(tower->eta());
    float Et = tower->et();

    if(!useNegTowers && Et < 0) continue;

    if(eta < 2.4){
      if(Et < et_max){
	rho += Et;
	length++;
      }
    }
  }
  float rho_bar = rho/length;

  return rho_bar;
}


/**
 *@brief Calculates rho as the median tower energy density in the barrel region of the calorimeter, to be scaled with area in more forward regions
 *@return @c float
 */
float Rho_med(const xAOD::JGTowerContainer* towers, const bool useNegTowers){
  float rho = 0;
  const unsigned int size = towers->size();
  std::vector<float> Et_in;

  for(unsigned int i = 0; i < size; i++){
    const xAOD::JGTower* tower = towers->at(i);
    float Et = tower->et();
    float eta = TMath::Abs(tower->eta());

    if(!useNegTowers && Et < 0) continue;

    if(eta < 2.4) Et_in.push_back(Et);
  }
  std::sort(Et_in.begin(), Et_in.end());
 
  if(Et_in.size() == 0) return 0;
  if(Et_in.size() == 1) rho = Et_in[0];
  else if(Et_in.size() % 2 == 0) rho = (Et_in[Et_in.size()/2] + Et_in[Et_in.size()/2 -1])/2;
  else rho = Et_in[Et_in.size()/2];

  return rho;
}

std::vector<float>* rhoSub(std::vector<const xAOD::JGTower*> towers, bool useNegTowers){
  float EtMiss = 0;
  float Ex = 0, Ey = 0, Ex_ = 0, Ey_ = 0;
  float threshold  = 0;

  //can calculate rho as either the average or median gTower energy in the barrel                                                            
  float rho = Rho_bar(towers, false);

  unsigned int size = towers.size();

  TH1F* h_Et = new TH1F("h_Et", "", 50, 0, 5000);

  for(unsigned int t = 0; t < size; t++){
    const xAOD::JGTower* tower = towers[t];
    float Et = tower->et();
    if(!useNegTowers && Et < 0) continue;
    h_Et->Fill(Et);
  }
  threshold = 3*h_Et->GetRMS();

  for(unsigned int t = 0; t < size; t++){
    const xAOD::JGTower* tower = towers[t];
    float Et = tower->et();
    float phi = tower->phi();
    float eta = TMath::Abs(tower->eta());

    float Et_sub = 0;
    if(eta < 2.4) Et_sub = TMath::Abs(Et) - rho;
    else Et_sub = TMath::Abs(Et) - 4*rho;

    if(Et_sub < threshold) continue;

    //if tower originally had negative Et, keep Et_sub negative                                                                              
    if(useNegTowers && Et < 0) Et_sub = -Et_sub;

    Ex_ = Et_sub*TMath::Cos(phi);
    Ey_ = Et_sub*TMath::Sin(phi);

    Ex += Ex_;
    Ey += Ey_;
  }

  EtMiss = TMath::Sqrt(Ex*Ex + Ey*Ey);
  float phi_met = 0;
  if(EtMiss != 0) phi_met = TMath::ACos(Ex/EtMiss);
  if (Ey<0) phi_met = -phi_met;
  
  std::vector<float>* met;
  met->push_back(EtMiss);
  met->push_back(phi_met);

  return met;
}

#endif
