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

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;
  virtual StatusCode fillVariablesPerOfflineMuonPerChain(const EventContext &ctx, const xAOD::Muon* mu, const std::string &chain) const override;

};

#endif //TRIGMUONMONITORINGMT_EFMUONMONMT_H
