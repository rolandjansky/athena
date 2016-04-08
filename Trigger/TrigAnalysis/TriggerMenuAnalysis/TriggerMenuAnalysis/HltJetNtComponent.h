/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_HLTJETNTCOMPONENT_H
#define TRIGGERMENUANALYSIS_HLTJETNTCOMPONENT_H
/*
  HltJetNtComponent.h
*/
#include "TrigNtupleBase/NtupleComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"
#include <vector>

class HltJetNtComponent : 
  public NtComponent::NtupleComponent {
public:
  HltJetNtComponent(NtupleAlgorithm* mainAlg, 
		   const NtComponent::NtComponentParameters& params);
  ~HltJetNtComponent();

  StatusCode book();
  StatusCode fill();

  void clear();

private:
  int m_NL2Jets;
  std::vector<float>* m_trigT2Jet_et;
  std::vector<float>* m_trigT2Jet_ehad0;
  std::vector<float>* m_trigT2Jet_eem0;
  std::vector<float>* m_trigT2Jet_eta;
  std::vector<float>* m_trigT2Jet_phi;

  int m_NEFJets;
  std::vector<float>* m_jet_EF_et;
  std::vector<float>* m_jet_EF_eta;
  std::vector<float>* m_jet_EF_phi;
};

#endif // TRIGGERMENUANALYSIS_HLTJETNTCOMPONENT_H
