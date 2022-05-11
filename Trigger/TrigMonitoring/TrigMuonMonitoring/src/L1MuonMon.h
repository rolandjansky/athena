/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORING_L1MUONMON_H
#define TRIGMUONMONITORING_L1MUONMON_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigger/MuonRoIContainer.h"


/*
This is a class for monitoring L1Muon.
 */
class L1MuonMon : public TrigMuonMonitorAlgorithm{

 public:
  L1MuonMon(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariables(const EventContext &ctx) const override;


 private:
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey {this, "MuonRoIContainerName", "LVL1MuonRoIs", "Level 1 muon container"};
  

};

#endif //TRIGMUONMONITORING_L1MUONMON_H
