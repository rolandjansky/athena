/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"


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
  SG::ReadHandleKey<xAOD::ElectronContainer> m_hlt_electron_key;
  SG::ReadHandleKey<xAOD::MuonContainer> m_hlt_muon_key;

  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_roi_key;
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_jnc_key;
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_jrho_key;
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_gnc_key;
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_grho_key;
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_gjwoj_key;
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1_gpufit_key;

  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_cell_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_mht_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_tc_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_tc_em_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_tcpufit_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_trkmht_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_pfsum_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_pfsum_cssk_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_pfsum_vssk_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_pfopufit_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_cvfpufit_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_mhtpufit_pf_met_key;
  SG::ReadHandleKey<xAOD::TrigMissingETContainer> m_hlt_mhtpufit_em_met_key;

  std::string m_L1Chain01;
  std::string m_L1Chain02;
  std::string m_L1Chain03;
  std::string m_L1Chain04;
  std::string m_L1Chain05;
  std::string m_L1Chain06;
  std::string m_L1Chain07;
  std::string m_HLTChain01;
  std::string m_HLTChain02;
  std::string m_HLTChain03;
  std::string m_HLTChain04;
  std::string m_HLTChain05;
  std::string m_HLTChain06;
  std::string m_HLTChain07;
  std::string m_HLTChain08;
  std::string m_HLTChain09;
  std::string m_HLTChain10;
  std::string m_HLTChain11;
  std::string m_HLTChain12;
  std::string m_HLTChain13;
  std::string m_HLTChain14;

  ToolHandle<Trig::ITrigDecisionTool> m_trigDecTool;
  

};
#endif
