/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigDecNtComponent_h__
#define __TrigDecNtComponent_h__
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
  TrigConf::ITrigConfigSvc* mTrigConfigSvc;
  Trig::TrigDecisionTool* mTrigDecisionTool;

  // Ntuple variable
  TrigDecSummary* mTrigDecSummary;

};

#endif // __TrigDecNtComponent_h__
