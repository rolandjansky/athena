/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNTRIGGERCOUNT_L1MUONMON_H
#define MUONNTRIGGERCOUNT_L1MUONMON_H

#include "TrigMuonMonitorAlgorithm.h"


/*
This is a class for monitoring L1Muon.
 */
class MuonTriggerCount : public TrigMuonMonitorAlgorithm{

 public:
  MuonTriggerCount(const std::string& name, ISvcLocator* pSvcLocator );

 protected:
  virtual StatusCode fillVariables(const EventContext &ctx) const override;

};

#endif //MUONNTRIGGERCOUNT_L1MUONMON_H
