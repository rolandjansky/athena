/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_EFMUONMONMT_H
#define TRIGMUONMONITORINGMT_EFMUONMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODMuon/MuonContainer.h"

/*
This is a class for monitoring EFMuon.
 */
class EFMuonMonMT : public TrigMuonMonitorAlgorithm{

 public:
  EFMuonMonMT(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;
  virtual StatusCode fillVariablesPerOfflineMuonPerChain(const EventContext &ctx, const xAOD::Muon* mu, const std::string &chain) const override;
  virtual StatusCode fillVariables(const EventContext& ctx) const override;
  virtual StatusCode fillVariablesPerOfflineMuon(const EventContext &ctx, const xAOD::Muon* mu) const override;

 private:
  SG::ReadHandleKey<xAOD::MuonContainer> m_EFSAMuonContainerKey {this, "EFSAMuonContainerName", "HLT_Muons_RoI", "EFSAMuon container"};
  SG::ReadHandleKey<xAOD::MuonContainer> m_EFCBMuonContainerKey {this, "EFCBMuonContainerName", "HLT_MuonsCB_RoI", "EFCBMuon container"};

};

#endif //TRIGMUONMONITORINGMT_EFMUONMONMT_H
