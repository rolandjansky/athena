/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ESNtComponent_h__
#define __ESNtComponent_h__
/*
  ESNtComponent.h
*/
#include "TrigNtupleBase/NtupleAlgorithm.h"
#include "TrigNtupleBase/NtupleComponent.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

class ESNtComponent : public NtComponent::NtupleComponent {
public:
  ESNtComponent(NtupleAlgorithm* algo, 
		const NtComponent::NtComponentParameters& params);
  ~ESNtComponent();

  StatusCode setupServices();
  StatusCode book();
  StatusCode fill();

  void clear();

protected:
  ServiceHandle<TrigConf::IHLTConfigSvc> mTrigConfigSvc;
  ToolHandle<Trig::TrigDecisionTool> mTDT;
  std::string mEventInfoKey;

  std::vector<std::string>* mChainNames;
  std::vector<int>* mChainCounters;
};

#endif // __ESNtComponent_h__
