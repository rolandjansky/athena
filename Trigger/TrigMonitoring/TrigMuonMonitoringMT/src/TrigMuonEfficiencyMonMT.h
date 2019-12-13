/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_TRIGMUONEFFICIENCYMONMT_H
#define TRIGMUONMONITORINGMT_TRIGMUONEFFICIENCYMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigger/MuonRoIContainer.h"

class MuonMatchingTool;

/*
This is a class for monitoring efficiency.
Analyzers are supposed to create one instance for one monitored trigger chain and configure every instance.
 */
class TrigMuonEfficiencyMonMT : public TrigMuonMonitorAlgorithm{

 public:
  TrigMuonEfficiencyMonMT(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  // virtual method
  virtual bool selectEvents() const override;
  virtual bool selectMuons(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const override;
  virtual StatusCode fillVariablesPerOfflineMuon(const EventContext &ctx, const xAOD::Muon* mu) const override;


  // tools
  ToolHandle<MuonMatchingTool> m_matchTool;

  // ReadHandles
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey;

  // properties
  Gaudi::Property<std::string> m_event_trigger {this, "EventTrigger", "", "Trigger used to select events"};
  Gaudi::Property<std::string> m_measured_trigger {this, "MeasuredTrigger", "", "Trigger that is measured"};
  Gaudi::Property<std::string> m_measured_trigger_L1 {this, "L1Seed", "", "L1 seed of measured trigger"};
  Gaudi::Property<std::string> m_group {this, "Group", "", "Histogram group"};
  Gaudi::Property<bool> m_doL2 {this, "doL2", true, "Flag to analyze L2"};
  Gaudi::Property<bool> m_doID {this, "doID", true, "Flag to analyze muComb and EFCB"};
  Gaudi::Property<bool> m_doIso {this, "doIso", true, "Flag to analyze muIso"};



};

#endif //TRIGMUONMONITORINGMT_TRIGMUONEFFICIENCYMONMT_H
