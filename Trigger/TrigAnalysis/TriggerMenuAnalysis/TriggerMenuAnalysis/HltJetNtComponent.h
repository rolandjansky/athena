/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HltJetNtComponent_h__
#define __HltJetNtComponent_h__
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
  int mNL2Jets;
  std::vector<float>* mTrigT2Jet_et;
  std::vector<float>* mTrigT2Jet_ehad0;
  std::vector<float>* mTrigT2Jet_eem0;
  std::vector<float>* mTrigT2Jet_eta;
  std::vector<float>* mTrigT2Jet_phi;

  int mNEFJets;
  std::vector<float>* mJet_EF_et;
  std::vector<float>* mJet_EF_eta;
  std::vector<float>* mJet_EF_phi;
};

#endif // __HltJetNtComponent_h__
