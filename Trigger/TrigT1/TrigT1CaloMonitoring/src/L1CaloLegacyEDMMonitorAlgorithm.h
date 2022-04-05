/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_L1CALOLEGACYEDMMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_L1CALOLEGACYEDMMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"

class L1CaloLegacyEDMMonitorAlgorithm : public AthMonitorAlgorithm {
public:
L1CaloLegacyEDMMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L1CaloLegacyEDMMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:

  StringProperty m_packageName{this,"PackageName","L1CaloLegacyEDMMonitor","group name for histograming"};
  SG::ReadHandleKey< xAOD::EmTauRoIContainer > m_l1TauRoIKey{this, "l1TauRoIKey","LVL1EmTauRoIs","Tau L1 RoI key"};
  SG::ReadHandleKey< xAOD::JetRoIContainer > m_l1JetRoIKey{this, "l1JetRoIKey","LVL1JetRoIs","Jet L1 RoI key"};
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_l1EnergySumRoIKey{this,"L1esumRoIName","LVL1EnergySumRoI","L1 Energy Sum ROI"};

};
#endif
