/**

 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

 */

#ifndef TRIGT1CALOFEXSIM_SOFTKILLER_H
#define TRIGT1CALOFEXSIM_SOFTKILLER_H

/*
 * Class  : Softkiller
 *
 * Author : Myers, Ava (amyers@cern.ch)
 *
 * Date   : Oct 2018 - Created class
 *
 * Implements the Softkiller algorithm in the context of MET reconstruction. Pileup suppression is applied in parallel to the threshold determined by the Softkiller algorithm.
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
#include "Identifier/IdentifierHash.h"
#include "TH1.h"
#include "TH2.h"
#include <vector>
#include "TMath.h"

/**
 * @brief Return a 2D histogram of 0.4x0.4 (eta x phi) patches containing the energy of the highest energy gTower within that patch
 * @return A @c TH2F*
 */
TH2F* ConstructGrid(const xAOD::JGTowerContainer* towers, bool useNegTowers){
  int NEta = 12;
  int NPhi = 16;
  
  TH2F* sk_grid = new TH2F("h_SKgrid","",NEta, -2.4, 2.4, NPhi, -3.14, 3.14);
  
  for(unsigned int t = 0; t < towers->size(); t++){
    const xAOD::JGTower* tower = towers->at(t);
    int ieta = sk_grid->GetXaxis()->FindBin(tower->eta());
    int iphi = sk_grid->GetYaxis()->FindBin(tower->phi());
    float Et_ = tower->et();
    
    if(!useNegTowers) Et_ = TMath::Abs(Et_);
    
    if(TMath::Abs(Et_) > sk_grid->GetBinContent(ieta, iphi)) sk_grid->SetBinContent(ieta, iphi, Et_);
    
  }
  return sk_grid;
}

/**
 * @brief Returns the true median of a vector. Used for validation
 * @return A @c float
 */
float Et_median_true(std::vector<float>* EtList){
  
  const int size = EtList->size();
  std::sort(EtList->begin(), EtList->end());
  
  float med_true;
  if(size == 0) med_true = 0.;
  else if( size == 1) med_true = EtList->at(0);
  else{
    if(size % 2 == 0) med_true = (EtList->at(size/2 - 1) + EtList->at(size/2))/2;
    else med_true = EtList->at(size/2);
  }
  return med_true;
}

/**
 * @brief Helper method for Et_median_approx
 * @return @c std::vector<float>
 */
std::vector<float> Et_median_approx_iteration(std::vector<float>* EtList, int nbins, float EtMin, float EtMax){
  
  int binUnderflow = 0;
  int binOverflow = 0;
  std::vector<int> bins(nbins, 0);

  float binWidth = (EtMax - EtMin)/nbins;
  float binOffset = EtMin;

  for(unsigned int t = 0; t < EtList->size(); t++){
    if(EtList->at(t) < EtMin) binUnderflow++;
    if(EtList->at(t) >= EtMax) binOverflow++;

    for(int bin = 0; bin < nbins; bin++){
      if(EtList->at(t) < (1. + bin)*binWidth + binOffset && EtList->at(t) >= EtMin) bins[bin]++;
    }
  }

  float newEtMin = -EtMin;
  float newEtMax = EtMax - EtMin; //in the case that no new value is selected the default value is retained following the addition of the offset

  int cutoff = ceil(EtList->size()/2.);  //finds 50% occupancy mark

  //set new max
  for(int bin = 0; bin < nbins; bin++){
    if(bins[nbins-1-bin] + binUnderflow > cutoff) newEtMax = (nbins-bin)*binWidth;
    if(bins[bin] + binUnderflow < cutoff) newEtMin = bin*binWidth;
  }

  newEtMin += binOffset;
  newEtMax += binOffset;

  std::vector<float> new_bounds;
  new_bounds.push_back(newEtMin);
  new_bounds.push_back(newEtMax);

  return new_bounds;
}

/**
 *@brief Return the approximate median of a vector. Iteratively calls Et_median_approx_iteration and returns the average of the constrained bounds returned by the last call
 *@return A @c float
 */
float Et_median_approx(std::vector<float>* EtList, int niterations, int nbins, float EtMin, float EtMax){
  for(int i = 0; i < niterations; i++){
    std::vector<float> new_bounds = Et_median_approx_iteration(EtList, nbins, EtMin, EtMax);
    EtMin = new_bounds[0];
    EtMax = new_bounds[1];
  }
  return (EtMax + EtMin)/2;
}

/**
 * @brief Converts the output of ConstructGrid to the input of Et_median_true
 * @return A @c std::vector<float>
 */
std::vector<float> FlattenHistogram(TH2F* h_grid){
  const int nEtaBins = h_grid->GetNbinsX();
  const int nPhiBins = h_grid->GetNbinsY();

  std::vector<float> EtMaxPerPatch;
  for(int ieta = 1; ieta <= nEtaBins; ieta++){
    for(int iphi = 1; iphi <= nPhiBins; iphi++){
      EtMaxPerPatch.push_back(h_grid->GetBinContent(ieta, iphi));
    }
  }
  return EtMaxPerPatch;
}

#endif
