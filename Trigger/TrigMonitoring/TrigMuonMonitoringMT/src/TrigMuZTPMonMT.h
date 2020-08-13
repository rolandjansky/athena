/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_TRIGMUZTPMONMT_H
#define TRIGMUONMONITORINGMT_TRIGMUZTPMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigger/MuonRoIContainer.h"

class MuonMatchingTool;

/*
This is a class for monitoring efficiency.
Analyzers are supposed to create one instance for one monitored trigger chain and configure every instance.
 */
class TrigMuZTPMonMT : public TrigMuonMonitorAlgorithm{

 public:
  TrigMuZTPMonMT(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  // virtual method
  virtual bool selectEvents() const override;
  virtual StatusCode selectMuons(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const override;
  virtual StatusCode fillVariablesPerOfflineMuonPerChain(const EventContext &ctx, const xAOD::Muon* mu, const std::string &chain) const override;


  // properties
  Gaudi::Property<std::string> m_event_trigger {this, "EventTrigger", "", "Trigger used to select events"};
  Gaudi::Property<std::string> m_tag_trig {this, "TagTrigger", "", "Trigger used to define tag muons"};
  Gaudi::Property<std::vector<std::string> > m_monitored_chains_L1 {this, "L1Seeds", {}, "List of L1 seeds of measured trigger"};
  Gaudi::Property<std::vector<double> > m_monitored_chains_threshold {this, "Thresholds", {}, "List of thresholds of measured trigger"};
  Gaudi::Property<bool> m_doL1 {this, "doL1", true, "Flag to analyze L1"};
  Gaudi::Property<bool> m_doL2SA {this, "doL2SA", true, "Flag to analyze L2MuonSA"};
  Gaudi::Property<bool> m_doL2CB {this, "doL2CB", true, "Flag to analyze L2muComb"};
  Gaudi::Property<bool> m_doEF {this, "doEF", true, "Flag to analyze EFMuon"};
  Gaudi::Property<bool> m_doEFIso {this, "doEFIso", true, "Flag to analyze EFIso"};

  // data
  std::map<std::string, std::string> m_l1seeds {};
  std::map<std::string, double> m_thresholds {};



};

#endif //TRIGMUONMONITORINGMT_TRIGMUZTPMONMT_H
