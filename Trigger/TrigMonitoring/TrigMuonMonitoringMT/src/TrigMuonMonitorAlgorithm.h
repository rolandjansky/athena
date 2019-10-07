/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_TRIGMUONMONITORALGORITHM_H
#define TRIGMUONMONITORINGMT_TRIGMUONMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "xAODMuon/MuonContainer.h"
#include "StoreGate/ReadHandleKey.h"



/*
This is a base class from which analyzers can define a derived class to do specific analysis.
e.g. L2MuonSAMonMT is such a class already defined.
Analyzers should define fillVariable and fillVariablePerOfflineMuon and these functions are automatically called in fillHistograms function.
Analyzers can change selectEvents and selectMuons in their specific classes by override them if needed.
TODO: function like fillVariablesPerChain that does per-chain analyses will be useful.
 */
class TrigMuonMonitorAlgorithm : public AthMonitorAlgorithm {

 public:
  TrigMuonMonitorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator );


  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext &ctx) const override;

 protected:
  virtual bool selectEvents() const;
  virtual bool selectMuons(SG::ReadHandle<xAOD::MuonContainer> &muons, std::vector<const xAOD::Muon*> &probes) const;
  virtual StatusCode fillVariables(const EventContext &ctx) const;
  virtual StatusCode fillVariablesPerOfflineMuon(const EventContext &ctx, const xAOD::Muon* mu) const;


  // ReadHandles
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey;
  

};

#endif //TRIGMUONMONITORINGMT_TRIGMUONMONITORALGORITHM_H
