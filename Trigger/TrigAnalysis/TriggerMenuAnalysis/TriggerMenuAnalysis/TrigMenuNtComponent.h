/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALSYIS_TRIGMENUNTCOMPONENT_H
#define TRIGGERMENUANALSYIS_TRIGMENUNTCOMPONENT_H
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
  Trig::TrigDecisionTool* m_trigDecisionTool;
  TrigAccessTool* m_trigAccessTool;
  RoILinksCnvTool* m_RoILinksCnvTool;

  // ntuple variables
  std::vector<ChainEntry>* m_chainEntries;
  RoILinks* m_RoILinks;

  // Variables used when using TrigDecisionTool
};


#endif // TRIGGERMENUANALSYIS_TRIGMENUNTCOMPONENT_H
