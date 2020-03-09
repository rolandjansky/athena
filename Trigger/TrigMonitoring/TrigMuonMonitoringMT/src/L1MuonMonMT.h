/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_L1MUONMONMT_H
#define TRIGMUONMONITORINGMT_L1MUONMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigger/MuonRoIContainer.h"


/*
This is a class for monitoring L1Muon.
 */
class L1MuonMonMT : public TrigMuonMonitorAlgorithm{

 public:
  L1MuonMonMT(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariables(const EventContext &ctx) const override;


 private:
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey;
  

};

#endif //TRIGMUONMONITORINGMT_L1MUONMONMT_H
