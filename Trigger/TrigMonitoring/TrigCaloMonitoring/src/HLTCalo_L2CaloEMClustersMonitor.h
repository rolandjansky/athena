/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOMONITORING_HLTCALO_L2CALOEMCLUSTERSMONITOR_H
#define TRIGCALOMONITORING_HLTCALO_L2CALOEMCLUSTERSMONITOR_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "LumiBlockData/BunchCrossingCondData.h"

class HLTCalo_L2CaloEMClustersMonitor : public AthMonitorAlgorithm {

public:

  HLTCalo_L2CaloEMClustersMonitor( const std::string& name, ISvcLocator* pSvcLocator );
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:
  float calculateDeltaR( float max_deltar, float eta_1, float phi_1, float eta_2, float phi_2 ) const;

  SG::ReadCondHandleKey<BunchCrossingCondData> m_bunchCrossingKey{this, "BunchCrossingKey", "BunchCrossingData", "Key BunchCrossing CDO" };
  SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_HLT_cont_key;
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_OFF_cont_key;
  std::string m_HLT_cont_name;
  std::string m_OFF_cont_name;
  std::string m_mongroup_name;
  float m_HLT_high_et;
  float m_HLT_min_et;
  float m_OFF_min_et;
  std::vector<int> m_OFF_types;
  float m_max_delta_r;

};
#endif
