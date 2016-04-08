/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_HLTCONFIGNTCOMPONENT_H
#define TRIGGERMENUANALYSIS_HLTCONFIGNTCOMPONENT_H
/*
  HltConfigNtComponent.h
*/
#include <vector>
#include <string>
#include "TrigNtupleBase/NtupleComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

class HltConfigNtComponent : public NtComponent::NtupleComponent {
public:
  HltConfigNtComponent(NtupleAlgorithm* algo, 
		       const NtComponent::NtComponentParameters& params);
  ~HltConfigNtComponent();

  StatusCode setupServices();
  StatusCode book();
  StatusCode fill();

  void clear();

protected:
  ServiceHandle<TrigConf::IHLTConfigSvc> m_trigConfigSvc;
  std::string m_eventInfoKey;

  int m_run;
  std::vector<std::string>* m_chainNames;
  std::vector<int>* m_chainCounters;
};

#endif // TRIGGERMENUANALYSIS_HLTCONFIGNTCOMPONENT_H
