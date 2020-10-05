/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNTRIGGERCOUNTMT_L1MUONMONMT_H
#define MUONNTRIGGERCOUNTMT_L1MUONMONMT_H

#include "TrigMuonMonitorAlgorithm.h"


/*
This is a class for monitoring L1Muon.
 */
class MuonTriggerCountMT : public TrigMuonMonitorAlgorithm{

 public:
  MuonTriggerCountMT(const std::string& name, ISvcLocator* pSvcLocator );

 protected:
  virtual StatusCode fillVariables(const EventContext &ctx) const override;

};

#endif //MUONNTRIGGERCOUNTMT_L1MUONMONMT_H
