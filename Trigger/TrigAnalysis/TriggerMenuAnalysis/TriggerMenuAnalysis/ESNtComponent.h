/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_ESNTCOMPONENT_H
#define TRIGGERMENUANALYSIS_ESNTCOMPONENT_H
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
  ServiceHandle<TrigConf::IHLTConfigSvc> m_trigConfigSvc;
  ToolHandle<Trig::TrigDecisionTool> m_TDT;
  std::string m_eventInfoKey;

  std::vector<std::string>* m_chainNames;
  std::vector<int>* m_chainCounters;
};

#endif // TRIGGERMENUANALYSIS_ESNTCOMPONENT_H
