/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_L1ROINTCOMPONENT_H
#define TRIGGERMENUANALYSIS_L1ROINTCOMPONENT_H
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
  std::vector<unsigned int>* m_L1MuonRoI_RoIWord;
  std::vector<float>* m_L1MuonRoI_pt;
  std::vector<float>* m_L1MuonRoI_eta;
  std::vector<float>* m_L1MuonRoI_phi;

  std::vector<unsigned int>* m_L1EmTauRoI_RoIWord;
  std::vector<float>* m_L1EmTauRoI_pt;
  std::vector<float>* m_L1EmTauRoI_eta;
  std::vector<float>* m_L1EmTauRoI_phi;
  std::vector<float>* m_L1EmTauRoI_Core;
  std::vector<float>* m_L1EmTauRoI_EmClus;
  std::vector<float>* m_L1EmTauRoI_TauClus;
  std::vector<float>* m_L1EmTauRoI_EmIsol;
  std::vector<float>* m_L1EmTauRoI_HadIsol;
  std::vector<float>* m_L1EmTauRoI_HadCore;

  std::vector<unsigned int>* m_L1JetRoI_RoIWord;
  std::vector<float>* m_L1JetRoI_pt;
  std::vector<float>* m_L1JetRoI_eta;
  std::vector<float>* m_L1JetRoI_phi;
  std::vector<float>* m_L1JetRoI_Et4x4;
  std::vector<float>* m_L1JetRoI_Et6x6;
  std::vector<float>* m_L1JetRoI_Et8x8;
};

#endif // TRIGGERMENUANALYSIS_L1ROINTCOMPONENT_H
