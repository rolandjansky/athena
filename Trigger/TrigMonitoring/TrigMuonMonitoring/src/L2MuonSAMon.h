/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORING_L2MUONSAMON_H
#define TRIGMUONMONITORING_L2MUONSAMON_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

/*
This is a class for monitoring L2MuonSA.
 */
class L2MuonSAMon : public TrigMuonMonitorAlgorithm{

 public:
  L2MuonSAMon(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;
  virtual StatusCode fillVariablesPerOfflineMuonPerChain(const EventContext& ctx, const xAOD::Muon* mu, const std::string &chain) const override;
  virtual StatusCode fillVariables(const EventContext& ctx) const override;
  virtual StatusCode fillVariablesPerOfflineMuon(const EventContext &ctx, const xAOD::Muon* mu) const override;

 private:
  SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_L2MuonSAContainerKey {this, "L2StandAloneMuonContainerName", "HLT_MuonL2SAInfo", "L2MuonSA container"};

};

#endif //TRIGMUONMONITORING_L2MUONSAMON_H
