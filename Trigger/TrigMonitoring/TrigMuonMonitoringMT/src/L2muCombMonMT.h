/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_L2MUCOMBMONMT_H
#define TRIGMUONMONITORINGMT_L2MUCOMBMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"

/*
This is a class for monitoring L2muComb.
 */
class L2muCombMonMT : public TrigMuonMonitorAlgorithm{

 public:
  L2muCombMonMT(const std::string& name, ISvcLocator* pSvcLocator );

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;
  virtual StatusCode fillVariablesPerOfflineMuonPerChain(const EventContext&, const xAOD::Muon* mu, const std::string &chain) const override;


};

#endif //TRIGMUONMONITORINGMT_L2MUCOMBMONMT_H
