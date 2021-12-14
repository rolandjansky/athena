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
  
  Gaudi::Property<std::vector<std::string>> m_ContainerNames{this, "ContainerNames", {}};
  
  Gaudi::Property<std::vector<std::string>> m_ChainNames_MuMu{this, "ChainNames_MuMu", {}};
  Gaudi::Property<std::vector<std::string>> m_ChainNames_MuMuX{this, "ChainNames_MuMuX", {}};
  
  StatusCode fillContainers(const EventContext& ctx) const;
  StatusCode fillContainerHists(const EventContext& ctx, const SG::ReadHandleKey<xAOD::TrigBphysContainer>& trigBphysContainerKey) const;
  
  StatusCode fillChains(const EventContext& ctx) const;
  StatusCode fillChainHistograms(const EventContext& ctx) const;
  StatusCode fillDimuonChainHists(const EventContext& ctx, const std::string& chainName) const;
  StatusCode fillBmumuxChainHists(const EventContext& ctx, const std::string& chainName) const;
  StatusCode fillChainGenericHists(const EventContext& /*ctx*/, const ToolHandle<GenericMonitoringTool>& currentMonGroup, const std::string& chainName) const;
  StatusCode fillBphysObjectHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer> bphysLink, const std::string& objStr) const;
  StatusCode fillMuonHists(const ToolHandle<GenericMonitoringTool>& currentMonGroup, const ElementLink<xAOD::TrigBphysContainer> bphysLink) const;
  
};
#endif
