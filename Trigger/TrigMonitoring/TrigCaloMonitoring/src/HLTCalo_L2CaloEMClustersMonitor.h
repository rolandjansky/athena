/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOMONITORING_HLTCALO_L2CALOEMCLUSTERSMONITOR_H
#define TRIGCALOMONITORING_HLTCALO_L2CALOEMCLUSTERSMONITOR_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class HLTCalo_L2CaloEMClustersMonitor : public AthMonitorAlgorithm {

public:

  HLTCalo_L2CaloEMClustersMonitor( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~HLTCalo_L2CaloEMClustersMonitor();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  virtual float calculateDeltaR( float eta_1, float phi_1, float eta_2, float phi_2 ) const;

private:

  SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_HLT_cont_key;
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_OFF_cont_key;
  std::string m_HLT_cont_name;
  std::string m_OFF_cont_name;
  std::string m_mongroup_name;
  float m_HLT_min_et;
  float m_OFF_min_et;
  std::vector<int> m_HLT_types;
  std::vector<int> m_OFF_types;
  float m_max_delta_r;

};
#endif
