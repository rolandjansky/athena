/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_L2MUONSAMONMT_H
#define TRIGMUONMONITORINGMT_L2MUONSAMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

class MuonMatchingTool;

/*
This is a class for monitoring L2MuonSA.
 */
class L2MuonSAMonMT : public TrigMuonMonitorAlgorithm{

 public:
  L2MuonSAMonMT(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;
  virtual StatusCode fillVariablesPerOfflineMuonPerChain(const EventContext& ctx, const xAOD::Muon* mu, const std::string &chain) const override;

  Gaudi::Property<std::string> m_group {this, "Group", "", "Histogram group"};

  ToolHandle<MuonMatchingTool> m_matchTool {this, "MuonMatchingTool", "MuonMatchingTool", "Tool for matching offline and online objects"};

};

#endif //TRIGMUONMONITORINGMT_L2MUONSAMONMT_H
