/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

#include "xAODEventInfo/EventInfo.h"

#include "TrigDecisionInterface/ITrigDecisionTool.h"

class TrigMETMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigMETMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  double signed_log(double e, double epsilon) const;

  SG::ReadHandleKey<xAOD::EventInfo> m_eventinfo_key;

  SG::ReadHandleKey<xAOD::MissingETContainer> m_offline_met_key;
  
  SG::ReadHandleKey<xAOD::ElectronContainer> m_hlt_electron_key;
  SG::ReadHandleKey<xAOD::MuonContainer> m_hlt_muon_key;

  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_topoclusters_key;
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracks_key;
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertex_key;

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

  std::string m_L1ChainA;
  std::string m_L1ChainB;
  std::string m_L1ChainC;
  std::string m_L1ChainD;
  std::string m_L1ChainE;
  std::string m_L1ChainF;
  std::string m_L1ChainG;
  std::string m_HLTChainA;
  std::string m_HLTChainB;
  std::string m_HLTChainC;
  std::string m_HLTChainD;
  std::string m_HLTChainE;
  std::string m_HLTChainF;
  std::string m_HLTChainG;
  std::string m_HLTChainH;
  std::string m_HLTChainI;
  std::string m_HLTChainJ;
  std::string m_HLTChainK;
  std::string m_HLTChainL;
  std::string m_HLTChainM;
  std::string m_HLTChainN;
};
#endif
