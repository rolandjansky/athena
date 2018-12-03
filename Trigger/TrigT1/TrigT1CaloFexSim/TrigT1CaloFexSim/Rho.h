/*

 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

Rho.h: Calculates energy density rho to subtract as pileup
Author: Ava Myers (amyers@cern.ch)
Methods:
Rho_bar(): Calculates rho as the average tower energy density in the barrel region of the calorimeter, to be scaled with area in more forward regions. An upper threshold of 3 GeV is applied to eliminate bias from hard scatter events

Rho_med(): Calculates rho as the median tower energy density in the barrel, again to be scaled with area

 */

#ifndef TRIGT1CALOFEXSIM_RHO_H
#define TRIGT1CALOFEXSIM_RHO_H

#include "CaloIdentifier/GTower_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/JGTower.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "Identifier/IdentifierHash.h"
#include <algorithm>
#include <vector>
#include "TMath.h"


float Rho_bar(const xAOD::JGTowerContainer* towers, bool useNegTowers){
  float rho = 0;
  float et_max = 3000;  //an upper threshold such that the average rho is not biased by hard scatter events
  const unsigned int size = towers->size();
  int length = 0;

  for(unsigned int i = 0; i < size; i++){
    const xAOD::JGTower* tower = towers->at(i);
    float eta = TMath::Abs(tower->eta());
    float Et = tower->et();

    if(!useNegTowers) Et = TMath::Abs(Et);
    
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

float Rho_med(const xAOD::JGTowerContainer* towers, bool useNegTowers){
  float rho = 0;
  const unsigned int size = towers->size();
  std::vector<float> Et_in;

  for(unsigned int i = 0; i < size; i++){
    const xAOD::JGTower* tower = towers->at(i);
    float Et = tower->et();
    float eta = TMath::Abs(tower->eta());
   
    if(!useNegTowers) Et = TMath::Abs(Et);

    if(eta < 2.4) Et_in.push_back(Et);
  }
  std::sort(Et_in.begin(), Et_in.end());

  if(Et_in.size() == 1) rho = Et_in.at(0);
  if(Et_in.size() % 2 == 0) rho = (Et_in.at(size/2) + Et_in.at(size/2 - 1))/2;
  else rho = Et_in.at(Et_in.size()/2);

  return rho;
}

#endif
