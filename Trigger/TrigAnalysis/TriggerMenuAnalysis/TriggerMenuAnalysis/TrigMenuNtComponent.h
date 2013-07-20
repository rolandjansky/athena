/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigMenuNtComponent_h__
#define __TrigMenuNtComponent_h__
/*
  TrigMenuNtComponent.h
*/
#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "TriggerMenuAnalysis/RoILinksCnvTool.h"


class TrigMenuNtComponent : public NtComponent::NtupleComponent {
public:
  TrigMenuNtComponent(NtupleAlgorithm* mainAlg, 
		      const NtComponent::NtComponentParameters& params);
  ~TrigMenuNtComponent();

  StatusCode book();
  StatusCode fill();

protected:
  void fillChain(const std::string& chain_name);
  std::vector<int> fillL2Muon(const std::string& chain_name);
  std::vector<int> fillL2Electron(const std::string& chain_name);
  void ClearMember();

private:
  Trig::TrigDecisionTool* mTrigDecisionTool;
  TrigAccessTool* mTrigAccessTool;
  RoILinksCnvTool* mRoILinksCnvTool;

  // ntuple variables
  std::vector<ChainEntry>* mChainEntries;
  RoILinks* mRoILinks;

  // Variables used when using TrigDecisionTool
};


#endif // __TrigMenuNtComponent_h__
