/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOMONITORING_HLTCALO_TOPOCALOCLUSTERSMONITOR_H
#define TRIGCALOMONITORING_HLTCALO_TOPOCALOCLUSTERSMONITOR_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "LumiBlockData/BunchCrossingCondData.h"
#include <math.h>

class HLTCalo_TopoCaloClustersMonitor : public AthMonitorAlgorithm {

public:

  HLTCalo_TopoCaloClustersMonitor( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~HLTCalo_TopoCaloClustersMonitor();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  virtual float calculateDeltaR( float max_deltar, float eta_1, float phi_1, float eta_2, float phi_2 ) const;
  virtual float calculateDeltaPhi( float phi_1, float phi_2 ) const;

private:

  SG::ReadCondHandleKey<BunchCrossingCondData> m_bunchCrossingKey{this, "BunchCrossingKey", "BunchCrossingData", "Key BunchCrossing CDO" };

  SG::ReadDecorHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "LArStatusFlag", "EventInfo.larFlag", "Key for EventInfo object"};
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_HLT_cont_key;
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_OFF_cont_key;
  std::string m_HLT_cont_name;
  std::string m_OFF_cont_name;
  std::string m_mongroup_name;
  float m_HLT_high_et;
  float m_HLT_min_et;
  float m_OFF_min_et;
  std::vector<int> m_HLT_types;
  std::vector<int> m_OFF_types;
  bool m_match_types;
  bool m_doLC;
  float m_max_delta_r;

};
#endif
