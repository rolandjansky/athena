/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMETMONITORING_TRIGMETMONITORALGORITHM_H
#define TRIGMETMONITORING_TRIGMETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTrigger/EnergySumRoI.h" 
#include "xAODTrigMissingET/TrigMissingETContainer.h" 
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h" 

#include "TrigDecisionInterface/ITrigDecisionTool.h"

class TrigMETMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigMETMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  double signed_log(double e, double epsilon) const;

  SG::ReadHandleKey<xAOD::MissingETContainer> m_offline_met_key;

  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_roi_key;

  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_cell_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_mht_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_tc_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_tcpufit_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_trkmht_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_pfsum_met_key;

  std::string m_L1Chain1;
  std::string m_HLTChain1;
  std::string m_HLTChain2;

  ToolHandle<Trig::ITrigDecisionTool> m_trigDecTool;
  

};
#endif
