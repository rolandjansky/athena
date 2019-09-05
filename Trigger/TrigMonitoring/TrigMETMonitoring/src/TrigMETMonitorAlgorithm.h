/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMETMONITORING_TRIGMETMONITORALGORITHM_H
#define TRIGMETMONITORING_TRIGMETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODTrigger/EnergySumRoI.h" 
#include "xAODTrigMissingET/TrigMissingETContainer.h" 
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h" 


class TrigMETMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigMETMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_roi_key;

  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_cell_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_mht_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_tc_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_tcpufit_met_key;

  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_run2_cell_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_run2_mht_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_run2_tc_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_run2_tcpufit_met_key;



};
#endif
