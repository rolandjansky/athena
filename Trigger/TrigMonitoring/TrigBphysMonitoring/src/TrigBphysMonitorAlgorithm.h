/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBPHYSMONITORING_TRIGBPHYSMONITORALGORITHM_H
#define TRIGBPHYSMONITORING_TRIGBPHYSMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODTrigBphys/TrigBphysContainer.h" 
#include "xAODTrigBphys/TrigBphys.h"

class TrigBphysMonitorAlgorithm : public AthMonitorAlgorithm {
public:
  TrigBphysMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigBphysMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  
private:
  SG::ReadHandleKeyArray<xAOD::TrigBphysContainer> m_TrigBphysContainerKeys;
  std::vector<std::string> m_MonitoredChains;
  std::vector<std::string> m_MonitoredContainers;
  ToolHandle<GenericMonitoringTool> m_mainGMT;
  
  StatusCode fillContainerHists(const EventContext& ctx) const;
  StatusCode fillChainHists(const EventContext& ctx) const;
  
  void fillContainer(const xAOD::TrigBphys* bphysObject,
                     const SG::ReadHandleKey<xAOD::TrigBphysContainer>& trigBphysContainerKey) const;
};
#endif
