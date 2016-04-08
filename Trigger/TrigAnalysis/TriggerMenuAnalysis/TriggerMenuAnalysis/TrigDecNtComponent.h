/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TRIGDECNTCOMPONENT_H
#define TRIGGERMENUANALYSIS_TRIGDECNTCOMPONENT_H
/*
  TrigDecNtComponent.h
*/
#include "TrigNtupleBase/NtupleComponent.h"
#include "TriggerMenuNtuple/TrigDecSummary.h"

namespace Trig {
  class TrigDecisionTool;
}
namespace TrigConf {
  class ITrigConfigSvc;
}
class TrigDecSummary;

class TrigDecNtComponent : public NtComponent::NtupleComponent {
public:
  TrigDecNtComponent(NtupleAlgorithm* mainAlg, 
		     const NtComponent::NtComponentParameters& params);
  ~TrigDecNtComponent();

  StatusCode book();
  StatusCode fill();

private:
  TrigConf::ITrigConfigSvc* m_trigConfigSvc;
  Trig::TrigDecisionTool* m_trigDecisionTool;

  // Ntuple variable
  TrigDecSummary* m_trigDecSummary;

};

#endif // TRIGGERMENUANALYSIS_TRIGDECNTCOMPONENT_H
