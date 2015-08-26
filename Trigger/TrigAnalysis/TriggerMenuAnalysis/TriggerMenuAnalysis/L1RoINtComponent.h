/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __L1RoINtComponent_h__
#define __L1RoINtComponent_h__
/*
  L1RoINtComponent.h
*/
#include "TrigNtupleBase/NtupleComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"
#include <vector>

class L1RoINtComponent : 
  public NtComponent::NtupleComponent {
public:
  L1RoINtComponent(NtupleAlgorithm* mainAlg, 
		   const NtComponent::NtComponentParameters& params);
  ~L1RoINtComponent();

  StatusCode book();
  StatusCode fill();

  void clear();

private:
  std::vector<unsigned int>* mL1MuonRoI_RoIWord;
  std::vector<float>* mL1MuonRoI_pt;
  std::vector<float>* mL1MuonRoI_eta;
  std::vector<float>* mL1MuonRoI_phi;

  std::vector<unsigned int>* mL1EmTauRoI_RoIWord;
  std::vector<float>* mL1EmTauRoI_pt;
  std::vector<float>* mL1EmTauRoI_eta;
  std::vector<float>* mL1EmTauRoI_phi;
  std::vector<float>* mL1EmTauRoI_Core;
  std::vector<float>* mL1EmTauRoI_EmClus;
  std::vector<float>* mL1EmTauRoI_TauClus;
  std::vector<float>* mL1EmTauRoI_EmIsol;
  std::vector<float>* mL1EmTauRoI_HadIsol;
  std::vector<float>* mL1EmTauRoI_HadCore;

  std::vector<unsigned int>* mL1JetRoI_RoIWord;
  std::vector<float>* mL1JetRoI_pt;
  std::vector<float>* mL1JetRoI_eta;
  std::vector<float>* mL1JetRoI_phi;
  std::vector<float>* mL1JetRoI_Et4x4;
  std::vector<float>* mL1JetRoI_Et6x6;
  std::vector<float>* mL1JetRoI_Et8x8;
};

#endif // __L1RoINtComponent_h__
