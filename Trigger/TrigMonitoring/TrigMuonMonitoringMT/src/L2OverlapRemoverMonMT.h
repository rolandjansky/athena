/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_L2OVERLAPREMOVERMONMT_H
#define TRIGMUONMONITORINGMT_L2OVERLAPREMOVERMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

/*
This is a class for monitoring L2MuonSA.
 */
class L2OverlapRemoverMonMT : public TrigMuonMonitorAlgorithm{

 public:
  L2OverlapRemoverMonMT(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;

};

#endif //TRIGMUONMONITORINGMT_L2OVERLAPREMOVERMONMT_H
