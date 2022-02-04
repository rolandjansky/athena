/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitorAlgorithm.h"
#include <TVector3.h>

TrigMETMonitorAlgorithm::TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty("eventinfo_key", m_eventinfo_key = "EventInfo");
  declareProperty("offline_met_key", m_offline_met_key = "MET_Reference_AntiKt4EMPFlow");
  declareProperty("hlt_electron_key", m_hlt_electron_key = "HLT_egamma_Electrons_GSF");
  declareProperty("hlt_muon_key", m_hlt_muon_key = "HLT_MuonsCB_FS");

  declareProperty("topoclusters_key", m_topoclusters_key = "HLT_TopoCaloClustersLCFS");
  declareProperty("tracks_key", m_tracks_key = "HLT_IDTrack_FS_FTF");
  declareProperty("vertex_key", m_vertex_key = "HLT_IDVertex_FS");

  declareProperty("l1_roi_key", m_lvl1_roi_key = "LVL1EnergySumRoI");
  declareProperty("l1_jnc_key", m_lvl1_jnc_key = "jNOISECUT_MET");
  declareProperty("l1_jrho_key", m_lvl1_jrho_key = "jXERHO_MET");
  declareProperty("l1_gnc_key", m_lvl1_gnc_key = "gXENOISECUT_MET");
  declareProperty("l1_grho_key", m_lvl1_grho_key = "gXERHO_MET");
  declareProperty("l1_gjwoj_key", m_lvl1_gjwoj_key = "gXEJWOJ_MET");
  declareProperty("l1_gpufit_key", m_lvl1_gpufit_key = "gXEPUFIT_MET");
  declareProperty("hlt_cell_key", m_hlt_cell_met_key = "HLT_MET_cell");
  declareProperty("hlt_mht_key", m_hlt_mht_met_key = "HLT_MET_mht");
  declareProperty("hlt_tc_key", m_hlt_tc_met_key = "HLT_MET_tc");
  declareProperty("hlt_tc_em_key", m_hlt_tc_em_met_key = "HLT_MET_tc_em");
  declareProperty("hlt_tcpufit_key", m_hlt_tcpufit_met_key = "HLT_MET_tcpufit");
  declareProperty("hlt_trkmht_key", m_hlt_trkmht_met_key = "HLT_MET_trkmht");
  declareProperty("hlt_pfsum_key", m_hlt_pfsum_met_key = "HLT_MET_pfsum");
  declareProperty("hlt_pfsum_cssk_key", m_hlt_pfsum_cssk_met_key = "HLT_MET_pfsum_cssk");
  declareProperty("hlt_pfsum_vssk_key", m_hlt_pfsum_vssk_met_key = "HLT_MET_pfsum_vssk");
  declareProperty("hlt_pfopufit_key", m_hlt_pfopufit_met_key = "HLT_MET_pfopufit");
  declareProperty("hlt_cvfpufit_key", m_hlt_cvfpufit_met_key = "HLT_MET_cvfpufit");
  declareProperty("hlt_mhtpufit_pf_key", m_hlt_mhtpufit_pf_met_key = "HLT_MET_mhtpufit_pf_subjesgscIS");
  declareProperty("hlt_mhtpufit_em_key", m_hlt_mhtpufit_em_met_key = "HLT_MET_mhtpufit_em_subjesgscIS");
}


TrigMETMonitorAlgorithm::~TrigMETMonitorAlgorithm() {}


StatusCode TrigMETMonitorAlgorithm::initialize() {
    ATH_CHECK( m_eventinfo_key.initialize() );
    ATH_CHECK( m_offline_met_key.initialize() );
    ATH_CHECK( m_hlt_electron_key.initialize() );
    ATH_CHECK( m_hlt_muon_key.initialize() );
    ATH_CHECK( m_topoclusters_key.initialize() );
    ATH_CHECK( m_tracks_key.initialize() );
    ATH_CHECK( m_vertex_key.initialize() );
    ATH_CHECK( m_lvl1_roi_key.initialize() );
    ATH_CHECK( m_lvl1_jnc_key.initialize() );
    ATH_CHECK( m_lvl1_jrho_key.initialize() );
    ATH_CHECK( m_lvl1_gnc_key.initialize() );
    ATH_CHECK( m_lvl1_grho_key.initialize() );
    ATH_CHECK( m_lvl1_gjwoj_key.initialize() );
    ATH_CHECK( m_lvl1_gpufit_key.initialize() );
    ATH_CHECK( m_hlt_cell_met_key.initialize() );
    ATH_CHECK( m_hlt_mht_met_key.initialize() );
    ATH_CHECK( m_hlt_tc_met_key.initialize() );
    ATH_CHECK( m_hlt_tc_em_met_key.initialize() );
    ATH_CHECK( m_hlt_tcpufit_met_key.initialize() );
    ATH_CHECK( m_hlt_trkmht_met_key.initialize() );
    ATH_CHECK( m_hlt_pfsum_met_key.initialize() );
    ATH_CHECK( m_hlt_pfopufit_met_key.initialize() );
    ATH_CHECK( m_hlt_cvfpufit_met_key.initialize() );
    ATH_CHECK( m_hlt_mhtpufit_pf_met_key.initialize() );
    ATH_CHECK( m_hlt_mhtpufit_em_met_key.initialize() );
    ATH_CHECK( m_hlt_pfsum_cssk_met_key.initialize() );
    ATH_CHECK( m_hlt_pfsum_vssk_met_key.initialize() );

    return AthMonitorAlgorithm::initialize();
}


StatusCode TrigMETMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // access event info container
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventinfo_key, ctx);
    if (! eventInfo.isValid() ){
        ATH_MSG_DEBUG("Container "<< eventInfo << " does not exist or is empty");
    }

    // access lepton containers
    SG::ReadHandle<xAOD::ElectronContainer> hlt_electron_cont(m_hlt_electron_key, ctx);
    if (! hlt_electron_cont.isValid() || hlt_electron_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_electron_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::MuonContainer> hlt_muon_cont(m_hlt_muon_key, ctx);
    if (! hlt_muon_cont.isValid() || hlt_muon_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_muon_key << " does not exist or is empty");
    }

    // access topoclusters container
    SG::ReadHandle<xAOD::CaloClusterContainer> hlt_topoclusters_cont(m_topoclusters_key, ctx);
    if (! hlt_topoclusters_cont.isValid() || hlt_topoclusters_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_topoclusters_key << " does not exist or is empty");
    }

    // access tracks container
    SG::ReadHandle<xAOD::TrackParticleContainer> hlt_tracks_cont(m_tracks_key, ctx);
    if (! hlt_tracks_cont.isValid() || hlt_tracks_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_tracks_key << " does not exist or is empty");
    }

    // access vertex container
    SG::ReadHandle<xAOD::VertexContainer> hlt_vertex_cont(m_vertex_key, ctx);
    if (! hlt_vertex_cont.isValid() || hlt_vertex_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_vertex_key << " does not exist or is empty");
    }

    // access offline met containers
    SG::ReadHandle<xAOD::MissingETContainer> offline_met_cont(m_offline_met_key, ctx);
    if (! offline_met_cont.isValid() || offline_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_offline_met_key << " does not exist or is empty");
    }

    // access L1 met containers
    SG::ReadHandle<xAOD::EnergySumRoI> l1_roi_cont(m_lvl1_roi_key, ctx);
    if (! l1_roi_cont.isValid() ) {
        ATH_MSG_DEBUG("Container "<< m_lvl1_roi_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::EnergySumRoI> l1_jnc_cont(m_lvl1_jnc_key, ctx);
    if (! l1_jnc_cont.isValid() ) {
        ATH_MSG_DEBUG("Container "<< m_lvl1_jnc_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::EnergySumRoI> l1_jrho_cont(m_lvl1_jrho_key, ctx);
    if (! l1_jrho_cont.isValid() ) {
        ATH_MSG_DEBUG("Container "<< m_lvl1_jrho_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::EnergySumRoI> l1_gnc_cont(m_lvl1_gnc_key, ctx);
    if (! l1_gnc_cont.isValid() ) {
        ATH_MSG_DEBUG("Container "<< m_lvl1_gnc_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::EnergySumRoI> l1_grho_cont(m_lvl1_grho_key, ctx);
    if (! l1_grho_cont.isValid() ) {
        ATH_MSG_DEBUG("Container "<< m_lvl1_grho_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::EnergySumRoI> l1_gjwoj_cont(m_lvl1_gjwoj_key, ctx);
    if (! l1_gjwoj_cont.isValid() ) {
        ATH_MSG_DEBUG("Container "<< m_lvl1_gjwoj_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::EnergySumRoI> l1_gpufit_cont(m_lvl1_gpufit_key, ctx);
    if (! l1_gpufit_cont.isValid() ) {
        ATH_MSG_DEBUG("Container "<< m_lvl1_gpufit_key << " does not exist or is empty");
    }

    // access HLT met containers
    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_cell_met_cont(m_hlt_cell_met_key, ctx);
    if (! hlt_cell_met_cont.isValid() || hlt_cell_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_cell_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_mht_met_cont(m_hlt_mht_met_key, ctx);
    if (! hlt_mht_met_cont.isValid() || hlt_mht_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_mht_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_tc_met_cont(m_hlt_tc_met_key, ctx);
    if (! hlt_tc_met_cont.isValid() || hlt_tc_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_tc_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_tc_em_met_cont(m_hlt_tc_em_met_key, ctx);
    if (! hlt_tc_em_met_cont.isValid() || hlt_tc_em_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_tc_em_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_tcpufit_met_cont(m_hlt_tcpufit_met_key, ctx);
    if (! hlt_tcpufit_met_cont.isValid() || hlt_tcpufit_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_tcpufit_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_trkmht_met_cont(m_hlt_trkmht_met_key, ctx);
    if (! hlt_trkmht_met_cont.isValid() || hlt_trkmht_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_trkmht_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_pfsum_met_cont(m_hlt_pfsum_met_key, ctx);
    if (! hlt_pfsum_met_cont.isValid() || hlt_pfsum_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_pfsum_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_pfsum_cssk_met_cont(m_hlt_pfsum_cssk_met_key, ctx);
    if (! hlt_pfsum_cssk_met_cont.isValid() || hlt_pfsum_cssk_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_pfsum_cssk_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_pfsum_vssk_met_cont(m_hlt_pfsum_vssk_met_key, ctx);
    if (! hlt_pfsum_vssk_met_cont.isValid() || hlt_pfsum_vssk_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_pfsum_vssk_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_pfopufit_met_cont(m_hlt_pfopufit_met_key, ctx);
    if (! hlt_pfopufit_met_cont.isValid() || hlt_pfopufit_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_pfopufit_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_cvfpufit_met_cont(m_hlt_cvfpufit_met_key, ctx);
    if (! hlt_cvfpufit_met_cont.isValid() || hlt_cvfpufit_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_cvfpufit_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_mhtpufit_pf_met_cont(m_hlt_mhtpufit_pf_met_key, ctx);
    if (! hlt_mhtpufit_pf_met_cont.isValid() || hlt_mhtpufit_pf_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_mhtpufit_pf_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_mhtpufit_em_met_cont(m_hlt_mhtpufit_em_met_key, ctx);
    if (! hlt_mhtpufit_em_met_cont.isValid() || hlt_mhtpufit_em_met_cont->size()==0 ) {
        ATH_MSG_DEBUG("Container "<< m_hlt_mhtpufit_em_met_key << " does not exist or is empty");
    }


    // define offline MissingET object
    const xAOD::MissingET *offline_met = 0;

    // define TrigMissingET object
    const xAOD::TrigMissingET *hlt_met = 0;

    // define variables
    auto act_IPBC = Monitored::Scalar<float>("act_IPBC",0.0);
    auto hlt_el_mult = Monitored::Scalar<int>("hlt_el_mult",0.0);
    auto hlt_el_pt = Monitored::Scalar<float>("hlt_el_pt",0.0);
    auto hlt_mu_mult = Monitored::Scalar<int>("hlt_mu_mult",0.0);
    auto hlt_mu_pt = Monitored::Scalar<float>("hlt_mu_pt",0.0);

    auto hlt_topoclusters_mult = Monitored::Scalar<int>("hlt_topoclusters_mult",0.0);
    auto hlt_topoclusters_pt = Monitored::Scalar<float>("hlt_topoclusters_pt",0.0);
    auto hlt_tracks_mult = Monitored::Scalar<int>("hlt_tracks_mult",0.0);
    auto hlt_tracks_pt = Monitored::Scalar<float>("hlt_tracks_pt",0.0);
    auto hlt_vertex_mult = Monitored::Scalar<int>("hlt_vertex_mult",0.0);
    auto hlt_vertex_z = Monitored::Scalar<float>("hlt_vertex_z",0.0);

    auto offline_Ex = Monitored::Scalar<float>("offline_Ex",0.0);
    auto offline_Ey = Monitored::Scalar<float>("offline_Ey",0.0);
    auto offline_Et = Monitored::Scalar<float>("offline_Et",0.0);
    auto offline_sumEt = Monitored::Scalar<float>("offline_sumEt",0.0);
    auto offline_Et_eff = Monitored::Scalar<float>("offline_Et_eff",0.0);
    auto L1_roi_Ex = Monitored::Scalar<float>("L1_roi_Ex",0.0);
    auto L1_roi_Ey = Monitored::Scalar<float>("L1_roi_Ey",0.0);
    auto L1_roi_Et = Monitored::Scalar<float>("L1_roi_Et",0.0);
    auto L1_roi_sumEt = Monitored::Scalar<float>("L1_roi_sumEt",0.0);
    auto L1_roi_Ex_log = Monitored::Scalar<float>("L1_roi_Ex_log",0.0);
    auto L1_roi_Ey_log = Monitored::Scalar<float>("L1_roi_Ey_log",0.0);
    auto L1_roi_Et_log = Monitored::Scalar<float>("L1_roi_Et_log",0.0);
    auto L1_roi_sumEt_log = Monitored::Scalar<float>("L1_roi_sumEt_log",0.0);
    auto L1_jnc_Ex = Monitored::Scalar<float>("L1_jnc_Ex",0.0);
    auto L1_jnc_Ey = Monitored::Scalar<float>("L1_jnc_Ey",0.0);
    auto L1_jnc_Et = Monitored::Scalar<float>("L1_jnc_Et",0.0);
    auto L1_jnc_sumEt = Monitored::Scalar<float>("L1_jnc_sumEt",0.0);
    auto L1_jnc_Ex_log = Monitored::Scalar<float>("L1_jnc_Ex_log",0.0);
    auto L1_jnc_Ey_log = Monitored::Scalar<float>("L1_jnc_Ey_log",0.0);
    auto L1_jnc_Et_log = Monitored::Scalar<float>("L1_jnc_Et_log",0.0);
    auto L1_jnc_sumEt_log = Monitored::Scalar<float>("L1_jnc_sumEt_log",0.0);
    auto L1_jrho_Ex = Monitored::Scalar<float>("L1_jrho_Ex",0.0);
    auto L1_jrho_Ey = Monitored::Scalar<float>("L1_jrho_Ey",0.0);
    auto L1_jrho_Et = Monitored::Scalar<float>("L1_jrho_Et",0.0);
    auto L1_jrho_sumEt = Monitored::Scalar<float>("L1_jrho_sumEt",0.0);
    auto L1_jrho_Ex_log = Monitored::Scalar<float>("L1_jrho_Ex_log",0.0);
    auto L1_jrho_Ey_log = Monitored::Scalar<float>("L1_jrho_Ey_log",0.0);
    auto L1_jrho_Et_log = Monitored::Scalar<float>("L1_jrho_Et_log",0.0);
    auto L1_jrho_sumEt_log = Monitored::Scalar<float>("L1_jrho_sumEt_log",0.0);
    auto L1_gnc_Ex = Monitored::Scalar<float>("L1_gnc_Ex",0.0);
    auto L1_gnc_Ey = Monitored::Scalar<float>("L1_gnc_Ey",0.0);
    auto L1_gnc_Et = Monitored::Scalar<float>("L1_gnc_Et",0.0);
    auto L1_gnc_sumEt = Monitored::Scalar<float>("L1_gnc_sumEt",0.0);
    auto L1_gnc_Ex_log = Monitored::Scalar<float>("L1_gnc_Ex_log",0.0);
    auto L1_gnc_Ey_log = Monitored::Scalar<float>("L1_gnc_Ey_log",0.0);
    auto L1_gnc_Et_log = Monitored::Scalar<float>("L1_gnc_Et_log",0.0);
    auto L1_gnc_sumEt_log = Monitored::Scalar<float>("L1_gnc_sumEt_log",0.0);
    auto L1_grho_Ex = Monitored::Scalar<float>("L1_grho_Ex",0.0);
    auto L1_grho_Ey = Monitored::Scalar<float>("L1_grho_Ey",0.0);
    auto L1_grho_Et = Monitored::Scalar<float>("L1_grho_Et",0.0);
    auto L1_grho_sumEt = Monitored::Scalar<float>("L1_grho_sumEt",0.0);
    auto L1_grho_Ex_log = Monitored::Scalar<float>("L1_grho_Ex_log",0.0);
    auto L1_grho_Ey_log = Monitored::Scalar<float>("L1_grho_Ey_log",0.0);
    auto L1_grho_Et_log = Monitored::Scalar<float>("L1_grho_Et_log",0.0);
    auto L1_grho_sumEt_log = Monitored::Scalar<float>("L1_grho_sumEt_log",0.0);
    auto L1_gjwoj_Ex = Monitored::Scalar<float>("L1_gjwoj_Ex",0.0);
    auto L1_gjwoj_Ey = Monitored::Scalar<float>("L1_gjwoj_Ey",0.0);
    auto L1_gjwoj_Et = Monitored::Scalar<float>("L1_gjwoj_Et",0.0);
    auto L1_gjwoj_sumEt = Monitored::Scalar<float>("L1_gjwoj_sumEt",0.0);
    auto L1_gjwoj_Ex_log = Monitored::Scalar<float>("L1_gjwoj_Ex_log",0.0);
    auto L1_gjwoj_Ey_log = Monitored::Scalar<float>("L1_gjwoj_Ey_log",0.0);
    auto L1_gjwoj_Et_log = Monitored::Scalar<float>("L1_gjwoj_Et_log",0.0);
    auto L1_gjwoj_sumEt_log = Monitored::Scalar<float>("L1_gjwoj_sumEt_log",0.0);
    auto L1_gpufit_Ex = Monitored::Scalar<float>("L1_gpufit_Ex",0.0);
    auto L1_gpufit_Ey = Monitored::Scalar<float>("L1_gpufit_Ey",0.0);
    auto L1_gpufit_Et = Monitored::Scalar<float>("L1_gpufit_Et",0.0);
    auto L1_gpufit_sumEt = Monitored::Scalar<float>("L1_gpufit_sumEt",0.0);
    auto L1_gpufit_Ex_log = Monitored::Scalar<float>("L1_gpufit_Ex_log",0.0);
    auto L1_gpufit_Ey_log = Monitored::Scalar<float>("L1_gpufit_Ey_log",0.0);
    auto L1_gpufit_Et_log = Monitored::Scalar<float>("L1_gpufit_Et_log",0.0);
    auto L1_gpufit_sumEt_log = Monitored::Scalar<float>("L1_gpufit_sumEt_log",0.0);
    auto cell_Ex = Monitored::Scalar<float>("cell_Ex",0.0);
    auto cell_Ey = Monitored::Scalar<float>("cell_Ey",0.0);
    auto cell_Et = Monitored::Scalar<float>("cell_Et",0.0);
    auto cell_sumEt = Monitored::Scalar<float>("cell_sumEt",0.0);
    auto cell_Ex_log = Monitored::Scalar<float>("cell_Ex_log",0.0);
    auto cell_Ey_log = Monitored::Scalar<float>("cell_Ey_log",0.0);
    auto cell_Et_log = Monitored::Scalar<float>("cell_Et_log",0.0);
    auto cell_sumEt_log = Monitored::Scalar<float>("cell_sumEt_log",0.0);
    auto cell_eta = Monitored::Scalar<float>("cell_eta",0.0);
    auto cell_phi = Monitored::Scalar<float>("cell_phi",0.0);
    auto mht_Ex = Monitored::Scalar<float>("mht_Ex",0.0);
    auto mht_Ey = Monitored::Scalar<float>("mht_Ey",0.0);
    auto mht_Et = Monitored::Scalar<float>("mht_Et",0.0);
    auto mht_sumEt = Monitored::Scalar<float>("mht_sumEt",0.0);
    auto mht_Ex_log = Monitored::Scalar<float>("mht_Ex_log",0.0);
    auto mht_Ey_log = Monitored::Scalar<float>("mht_Ey_log",0.0);
    auto mht_Et_log = Monitored::Scalar<float>("mht_Et_log",0.0);
    auto mht_sumEt_log = Monitored::Scalar<float>("mht_sumEt_log",0.0);
    auto mht_eta = Monitored::Scalar<float>("mht_eta",0.0);
    auto mht_phi = Monitored::Scalar<float>("mht_phi",0.0);
    auto tc_em_Ex = Monitored::Scalar<float>("tc_em_Ex",0.0);
    auto tc_em_Ey = Monitored::Scalar<float>("tc_em_Ey",0.0);
    auto tc_em_Et = Monitored::Scalar<float>("tc_em_Et",0.0);
    auto tc_em_sumEt = Monitored::Scalar<float>("tc_em_sumEt",0.0);
    auto tc_em_Ex_log = Monitored::Scalar<float>("tc_em_Ex_log",0.0);
    auto tc_em_Ey_log = Monitored::Scalar<float>("tc_em_Ey_log",0.0);
    auto tc_em_Et_log = Monitored::Scalar<float>("tc_em_Et_log",0.0);
    auto tc_em_sumEt_log = Monitored::Scalar<float>("tc_em_sumEt_log",0.0);
    auto tc_em_eta = Monitored::Scalar<float>("tc_em_eta",0.0);
    auto tc_em_phi = Monitored::Scalar<float>("tc_em_phi",0.0);
    auto tc_Ex = Monitored::Scalar<float>("tc_Ex",0.0);
    auto tc_Ey = Monitored::Scalar<float>("tc_Ey",0.0);
    auto tc_Et = Monitored::Scalar<float>("tc_Et",0.0);
    auto trkmht_Ex = Monitored::Scalar<float>("trkmht_Ex",0.0);
    auto trkmht_Ey = Monitored::Scalar<float>("trkmht_Ey",0.0);
    auto trkmht_Et = Monitored::Scalar<float>("trkmht_Et",0.0);
    auto trkmht_sumEt = Monitored::Scalar<float>("trkmht_sumEt",0.0);
    auto trkmht_Ex_log = Monitored::Scalar<float>("trkmht_Ex_log",0.0);
    auto trkmht_Ey_log = Monitored::Scalar<float>("trkmht_Ey_log",0.0);
    auto trkmht_Et_log = Monitored::Scalar<float>("trkmht_Et_log",0.0);
    auto trkmht_sumEt_log = Monitored::Scalar<float>("trkmht_sumEt_log",0.0);
    auto trkmht_phi = Monitored::Scalar<float>("trkmht_phi",0.0);
    auto tcpufit_Ex = Monitored::Scalar<float>("tcpufit_Ex",0.0);
    auto tcpufit_Ey = Monitored::Scalar<float>("tcpufit_Ey",0.0);
    auto tcpufit_Ez = Monitored::Scalar<float>("tcpufit_Ez",0.0);
    auto tcpufit_Et = Monitored::Scalar<float>("tcpufit_Et",0.0);
    auto tcpufit_sumEt = Monitored::Scalar<float>("tcpufit_sumEt",0.0);
    auto tcpufit_sumE = Monitored::Scalar<float>("tcpufit_sumE",0.0);
    auto tcpufit_Ex_log = Monitored::Scalar<float>("tcpufit_Ex_log",0.0);
    auto tcpufit_Ey_log = Monitored::Scalar<float>("tcpufit_Ey_log",0.0);
    auto tcpufit_Ez_log = Monitored::Scalar<float>("tcpufit_Ez_log",0.0);
    auto tcpufit_Et_log = Monitored::Scalar<float>("tcpufit_Et_log",0.0);
    auto tcpufit_sumEt_log = Monitored::Scalar<float>("tcpufit_sumEt_log",0.0);
    auto tcpufit_sumE_log = Monitored::Scalar<float>("tcpufit_sumE_log",0.0);
    auto tcpufit_eta = Monitored::Scalar<float>("tcpufit_eta",0.0);
    auto tcpufit_phi = Monitored::Scalar<float>("tcpufit_phi",0.0);
    auto pfsum_Ex = Monitored::Scalar<float>("pfsum_Ex",0.0);
    auto pfsum_Ey = Monitored::Scalar<float>("pfsum_Ey",0.0);
    auto pfsum_Et = Monitored::Scalar<float>("pfsum_Et",0.0);
    auto pfsum_sumEt = Monitored::Scalar<float>("pfsum_sumEt",0.0);
    auto pfsum_Ex_log = Monitored::Scalar<float>("pfsum_Ex_log",0.0);
    auto pfsum_Ey_log = Monitored::Scalar<float>("pfsum_Ey_log",0.0);
    auto pfsum_Et_log = Monitored::Scalar<float>("pfsum_Et_log",0.0);
    auto pfsum_sumEt_log = Monitored::Scalar<float>("pfsum_sumEt_log",0.0);
    auto pfsum_phi = Monitored::Scalar<float>("pfsum_phi",0.0);
    auto pfsum_cssk_Ex = Monitored::Scalar<float>("pfsum_cssk_Ex",0.0);
    auto pfsum_cssk_Ey = Monitored::Scalar<float>("pfsum_cssk_Ey",0.0);
    auto pfsum_cssk_Et = Monitored::Scalar<float>("pfsum_cssk_Et",0.0);
    auto pfsum_cssk_sumEt = Monitored::Scalar<float>("pfsum_cssk_sumEt",0.0);
    auto pfsum_cssk_Ex_log = Monitored::Scalar<float>("pfsum_cssk_Ex_log",0.0);
    auto pfsum_cssk_Ey_log = Monitored::Scalar<float>("pfsum_cssk_Ey_log",0.0);
    auto pfsum_cssk_Et_log = Monitored::Scalar<float>("pfsum_cssk_Et_log",0.0);
    auto pfsum_cssk_sumEt_log = Monitored::Scalar<float>("pfsum_cssk_sumEt_log",0.0);
    auto pfsum_cssk_phi = Monitored::Scalar<float>("pfsum_cssk_phi",0.0);
    auto pfsum_vssk_Ex = Monitored::Scalar<float>("pfsum_vssk_Ex",0.0);
    auto pfsum_vssk_Ey = Monitored::Scalar<float>("pfsum_vssk_Ey",0.0);
    auto pfsum_vssk_Et = Monitored::Scalar<float>("pfsum_vssk_Et",0.0);
    auto pfsum_vssk_sumEt = Monitored::Scalar<float>("pfsum_vssk_sumEt",0.0);
    auto pfsum_vssk_Ex_log = Monitored::Scalar<float>("pfsum_vssk_Ex_log",0.0);
    auto pfsum_vssk_Ey_log = Monitored::Scalar<float>("pfsum_vssk_Ey_log",0.0);
    auto pfsum_vssk_Et_log = Monitored::Scalar<float>("pfsum_vssk_Et_log",0.0);
    auto pfsum_vssk_sumEt_log = Monitored::Scalar<float>("pfsum_vssk_sumEt_log",0.0);
    auto pfsum_vssk_phi = Monitored::Scalar<float>("pfsum_vssk_phi",0.0);
    auto pfopufit_Ex = Monitored::Scalar<float>("pfopufit_Ex",0.0);
    auto pfopufit_Ey = Monitored::Scalar<float>("pfopufit_Ey",0.0);
    auto pfopufit_Et = Monitored::Scalar<float>("pfopufit_Et",0.0);
    auto pfopufit_sumEt = Monitored::Scalar<float>("pfopufit_sumEt",0.0);
    auto pfopufit_Ex_log = Monitored::Scalar<float>("pfopufit_Ex_log",0.0);
    auto pfopufit_Ey_log = Monitored::Scalar<float>("pfopufit_Ey_log",0.0);
    auto pfopufit_Et_log = Monitored::Scalar<float>("pfopufit_Et_log",0.0);
    auto pfopufit_sumEt_log = Monitored::Scalar<float>("pfopufit_sumEt_log",0.0);
    auto pfopufit_phi = Monitored::Scalar<float>("pfopufit_phi",0.0);
    auto pfopufit_eta = Monitored::Scalar<float>("pfopufit_eta",0.0);
    auto cvfpufit_Ex = Monitored::Scalar<float>("cvfpufit_Ex",0.0);
    auto cvfpufit_Ey = Monitored::Scalar<float>("cvfpufit_Ey",0.0);
    auto cvfpufit_Et = Monitored::Scalar<float>("cvfpufit_Et",0.0);
    auto cvfpufit_sumEt = Monitored::Scalar<float>("cvfpufit_sumEt",0.0);
    auto cvfpufit_Ex_log = Monitored::Scalar<float>("cvfpufit_Ex_log",0.0);
    auto cvfpufit_Ey_log = Monitored::Scalar<float>("cvfpufit_Ey_log",0.0);
    auto cvfpufit_Et_log = Monitored::Scalar<float>("cvfpufit_Et_log",0.0);
    auto cvfpufit_sumEt_log = Monitored::Scalar<float>("cvfpufit_sumEt_log",0.0);
    auto cvfpufit_phi = Monitored::Scalar<float>("cvfpufit_phi",0.0);
    auto mhtpufit_pf_Ex = Monitored::Scalar<float>("mhtpufit_pf_Ex",0.0);
    auto mhtpufit_pf_Ey = Monitored::Scalar<float>("mhtpufit_pf_Ey",0.0);
    auto mhtpufit_pf_Et = Monitored::Scalar<float>("mhtpufit_pf_Et",0.0);
    auto mhtpufit_pf_sumEt = Monitored::Scalar<float>("mhtpufit_pf_sumEt",0.0);
    auto mhtpufit_pf_Ex_log = Monitored::Scalar<float>("mhtpufit_pf_Ex_log",0.0);
    auto mhtpufit_pf_Ey_log = Monitored::Scalar<float>("mhtpufit_pf_Ey_log",0.0);
    auto mhtpufit_pf_Et_log = Monitored::Scalar<float>("mhtpufit_pf_Et_log",0.0);
    auto mhtpufit_pf_sumEt_log = Monitored::Scalar<float>("mhtpufit_pf_sumEt_log",0.0);
    auto mhtpufit_pf_phi = Monitored::Scalar<float>("mhtpufit_pf_phi",0.0);
    auto mhtpufit_em_Ex = Monitored::Scalar<float>("mhtpufit_em_Ex",0.0);
    auto mhtpufit_em_Ey = Monitored::Scalar<float>("mhtpufit_em_Ey",0.0);
    auto mhtpufit_em_Et = Monitored::Scalar<float>("mhtpufit_em_Et",0.0);
    auto mhtpufit_em_sumEt = Monitored::Scalar<float>("mhtpufit_em_sumEt",0.0);
    auto mhtpufit_em_Ex_log = Monitored::Scalar<float>("mhtpufit_em_Ex_log",0.0);
    auto mhtpufit_em_Ey_log = Monitored::Scalar<float>("mhtpufit_em_Ey_log",0.0);
    auto mhtpufit_em_Et_log = Monitored::Scalar<float>("mhtpufit_em_Et_log",0.0);
    auto mhtpufit_em_sumEt_log = Monitored::Scalar<float>("mhtpufit_em_sumEt_log",0.0);
    auto mhtpufit_em_phi = Monitored::Scalar<float>("mhtpufit_em_phi",0.0);

    //< -- Preselection histogram quantities -- > //
    auto cell_presel_Et = Monitored::Scalar<float>("cell_presel_Et",0.0);
    auto tcpufit_presel_Et = Monitored::Scalar<float>("tcpufit_presel_Et",0.0);
    auto trkmht_presel_Et = Monitored::Scalar<float>("trkmht_presel_Et",0.0);
    auto mht_presel_Et = Monitored::Scalar<float>("mht_presel_Et",0.0);
    auto tc_em_presel_Et = Monitored::Scalar<float>("tc_em_presel_Et",0.0);
    auto pfsum_presel_Et = Monitored::Scalar<float>("pfsum_presel_Et",0.0);
    auto pfsum_cssk_presel_Et = Monitored::Scalar<float>("pfsum_cssk_presel_Et",0.0);
    auto pfsum_vssk_presel_Et = Monitored::Scalar<float>("pfsum_vssk_presel_Et",0.0);
    auto pfopufit_presel_Et = Monitored::Scalar<float>("pfopufit_presel_Et",0.0);
    auto cvfpufit_presel_Et = Monitored::Scalar<float>("cvfpufit_presel_Et",0.0);
    auto mhtpufit_pf_presel_Et = Monitored::Scalar<float>("mhtpufit_pf_presel_Et",0.0);
    auto mhtpufit_em_presel_Et = Monitored::Scalar<float>("mhtpufit_em_presel_Et",0.0);

    auto HLT_MET_status = Monitored::Scalar<int>("HLT_MET_status",0.0);
    auto MET_status = Monitored::Scalar<float>("MET_status",0.0);
    auto HLT_MET_component = Monitored::Scalar<int>("HLT_MET_component",0.0);
    auto component_Et = Monitored::Scalar<float>("component_Et",0.0);
    auto component = Monitored::Scalar<int>("component",0.0);
    auto component_status = Monitored::Scalar<int>("component_status",0.0);
    auto component_status_weight = Monitored::Scalar<int>("component_status_weight",0.0);

    // constant floor for log plots
    double epsilon = 1.189;

    // for histogram filling
    auto tool = getGroup("TrigMETMonitor");

    act_IPBC = eventInfo->actualInteractionsPerCrossing();
    fill(tool, act_IPBC);

    // access lepton values
    if ( hlt_electron_cont.isValid() && hlt_electron_cont->size() > 0 ) {
      hlt_el_mult = hlt_electron_cont->size();
      for (auto Electron: *hlt_electron_cont) {
        hlt_el_pt = (Electron->pt())*0.001;
        if (hlt_el_pt > 0) {
          fill(tool, hlt_el_pt);
        }
      }
    }

    if( hlt_muon_cont.isValid() && hlt_muon_cont->size() > 0){
      hlt_mu_mult = hlt_muon_cont->size();
      for(auto Muon : *hlt_muon_cont){
        hlt_mu_pt = (Muon->pt())*0.001;
        if(hlt_mu_pt > 0){
          fill(tool, hlt_mu_pt);
        }
      }
    }

    // access topoclusters container
    if(hlt_topoclusters_cont.isValid() && hlt_topoclusters_cont->size() > 0){
      hlt_topoclusters_mult = hlt_topoclusters_cont->size();
      for(auto topoclusters : *hlt_topoclusters_cont){
        hlt_topoclusters_pt = (topoclusters->pt())*0.001;
        if(hlt_topoclusters_pt > 0){
          fill(tool, hlt_topoclusters_pt);
        }
      }

      if(hlt_topoclusters_mult > 0){
        fill(tool, hlt_topoclusters_mult);
      }
    }

    // access tracks container
    if( hlt_tracks_cont.isValid() && hlt_tracks_cont->size() > 0){
      hlt_tracks_mult = hlt_tracks_cont->size();
      for(auto tracks : *hlt_tracks_cont){
        hlt_tracks_pt = (tracks->pt())*0.001;
        if(hlt_tracks_pt > 0){
          fill(tool, hlt_tracks_pt);
        }
      }

      if(hlt_tracks_mult > 0){
        fill(tool, hlt_tracks_mult);
      }
    }

    // access vertex container
    if(hlt_vertex_cont.isValid() && hlt_vertex_cont->size() > 0){
      hlt_vertex_mult = hlt_vertex_cont->size();

      if(hlt_vertex_mult > 0){
        auto hs_vertex = hlt_vertex_cont->at(0);
        hlt_vertex_z = hs_vertex->z();

        fill(tool, hlt_vertex_mult, hlt_vertex_z);
      }
    }
        
    // access offline MET values
    if ( offline_met_cont.isValid() && offline_met_cont->size() > 0 ) {
      offline_met = offline_met_cont->at(0);
      offline_Ex = - (offline_met->mpx())/1000.;
      offline_Ey = - (offline_met->mpy())/1000.;
      offline_sumEt = (offline_met->sumet())/1000.;
      offline_Et = std::sqrt(offline_Ex*offline_Ex + offline_Ey*offline_Ey);
      offline_Et_eff = std::sqrt(offline_Ex*offline_Ex + offline_Ey*offline_Ey);
    }

    // access L1 roi MET values
    if ( l1_roi_cont.isValid() ) {
      if ((l1_roi_cont->energyX())>-9e12 && (l1_roi_cont->energyX())<9e12 && (l1_roi_cont->energyY())>-9e12 && (l1_roi_cont->energyY())<9e12) {
	      L1_roi_Ex = - (l1_roi_cont->energyX())/1000.;
	      L1_roi_Ey = - (l1_roi_cont->energyY())/1000.;
        L1_roi_Et = std::sqrt(L1_roi_Ex*L1_roi_Ex + L1_roi_Ey*L1_roi_Ey);
        L1_roi_sumEt = (l1_roi_cont->energyT())/1000.;
        L1_roi_Ex_log = signed_log(L1_roi_Ex, epsilon);
        L1_roi_Ey_log = signed_log(L1_roi_Ey, epsilon);
        L1_roi_Et_log = signed_log(L1_roi_Et, epsilon);
        L1_roi_sumEt_log = signed_log(L1_roi_sumEt, epsilon);
      }
    }

    // access L1 jnc MET values
    if ( l1_jnc_cont.isValid() ) {
      if ((l1_jnc_cont->energyX())>-9e12 && (l1_jnc_cont->energyX())<9e12 && (l1_jnc_cont->energyY())>-9e12 && (l1_jnc_cont->energyY())<9e12) {
        L1_jnc_Ex = - (l1_jnc_cont->energyX())/1000.;
        L1_jnc_Ey = - (l1_jnc_cont->energyY())/1000.;
        L1_jnc_Et = std::sqrt(L1_jnc_Ex*L1_jnc_Ex + L1_jnc_Ey*L1_jnc_Ey);
        L1_jnc_sumEt = (l1_jnc_cont->energyT())/1000.;
        L1_jnc_Ex_log = signed_log(L1_jnc_Ex, epsilon);
        L1_jnc_Ey_log = signed_log(L1_jnc_Ey, epsilon);
        L1_jnc_Et_log = signed_log(L1_jnc_Et, epsilon);
        L1_jnc_sumEt_log = signed_log(L1_jnc_sumEt, epsilon);
      }
    }

    // access L1 jrho MET values
    if ( l1_jrho_cont.isValid() ) {
      if ((l1_jrho_cont->energyX())>-9e12 && (l1_jrho_cont->energyX())<9e12 && (l1_jrho_cont->energyY())>-9e12 && (l1_jrho_cont->energyY())<9e12) {
        L1_jrho_Ex = - (l1_jrho_cont->energyX())/1000.;
        L1_jrho_Ey = - (l1_jrho_cont->energyY())/1000.;
        L1_jrho_Et = std::sqrt(L1_jrho_Ex*L1_jrho_Ex + L1_jrho_Ey*L1_jrho_Ey);
        L1_jrho_sumEt = (l1_jrho_cont->energyT())/1000.;
        L1_jrho_Ex_log = signed_log(L1_jrho_Ex, epsilon);
        L1_jrho_Ey_log = signed_log(L1_jrho_Ey, epsilon);
        L1_jrho_Et_log = signed_log(L1_jrho_Et, epsilon);
        L1_jrho_sumEt_log = signed_log(L1_jrho_sumEt, epsilon);
      }
    }

    // access L1 gnc MET values
    if ( l1_gnc_cont.isValid() ) {
      if ((l1_gnc_cont->energyX())>-9e12 && (l1_gnc_cont->energyX())<9e12 && (l1_gnc_cont->energyY())>-9e12 && (l1_gnc_cont->energyY())<9e12) {
        L1_gnc_Ex = - (l1_gnc_cont->energyX())/1000.;
        L1_gnc_Ey = - (l1_gnc_cont->energyY())/1000.;
        L1_gnc_Et = std::sqrt(L1_gnc_Ex*L1_gnc_Ex + L1_gnc_Ey*L1_gnc_Ey);
        L1_gnc_sumEt = (l1_gnc_cont->energyT())/1000.;
        L1_gnc_Ex_log = signed_log(L1_gnc_Ex, epsilon);
        L1_gnc_Ey_log = signed_log(L1_gnc_Ey, epsilon);
        L1_gnc_Et_log = signed_log(L1_gnc_Et, epsilon);
        L1_gnc_sumEt_log = signed_log(L1_gnc_sumEt, epsilon);
      }
    }

    // access L1 grho MET values
    if ( l1_grho_cont.isValid() ) {
      if ((l1_grho_cont->energyX())>-9e12 && (l1_grho_cont->energyX())<9e12 && (l1_grho_cont->energyY())>-9e12 && (l1_grho_cont->energyY())<9e12) {
        L1_grho_Ex = - (l1_grho_cont->energyX())/1000.;
        L1_grho_Ey = - (l1_grho_cont->energyY())/1000.;
        L1_grho_Et = std::sqrt(L1_grho_Ex*L1_grho_Ex + L1_grho_Ey*L1_grho_Ey);
        L1_grho_sumEt = (l1_grho_cont->energyT())/1000.;
        L1_grho_Ex_log = signed_log(L1_grho_Ex, epsilon);
        L1_grho_Ey_log = signed_log(L1_grho_Ey, epsilon);
        L1_grho_Et_log = signed_log(L1_grho_Et, epsilon);
        L1_grho_sumEt_log = signed_log(L1_grho_sumEt, epsilon);
      }
    }

    // access L1 gjwoj MET values
    if ( l1_gjwoj_cont.isValid() ) {
      if ((l1_gjwoj_cont->energyX())>-9e12 && (l1_gjwoj_cont->energyX())<9e12 && (l1_gjwoj_cont->energyY())>-9e12 && (l1_gjwoj_cont->energyY())<9e12) {
        L1_gjwoj_Ex = - (l1_gjwoj_cont->energyX())/1000.;
        L1_gjwoj_Ey = - (l1_gjwoj_cont->energyY())/1000.;
        L1_gjwoj_Et = std::sqrt(L1_gjwoj_Ex*L1_gjwoj_Ex + L1_gjwoj_Ey*L1_gjwoj_Ey);
        L1_gjwoj_sumEt = (l1_gjwoj_cont->energyT())/1000.;
        L1_gjwoj_Ex_log = signed_log(L1_gjwoj_Ex, epsilon);
        L1_gjwoj_Ey_log = signed_log(L1_gjwoj_Ey, epsilon);
        L1_gjwoj_Et_log = signed_log(L1_gjwoj_Et, epsilon);
        L1_gjwoj_sumEt_log = signed_log(L1_gjwoj_sumEt, epsilon);
      }
    }

    // access L1 gpufit MET values
    if ( l1_gpufit_cont.isValid() ) {
      if ((l1_gpufit_cont->energyX())>-9e12 && (l1_gpufit_cont->energyX())<9e12 && (l1_gpufit_cont->energyY())>-9e12 && (l1_gpufit_cont->energyY())<9e12) {
        L1_gpufit_Ex = - (l1_gpufit_cont->energyX())/1000.;
        L1_gpufit_Ey = - (l1_gpufit_cont->energyY())/1000.;
        L1_gpufit_Et = std::sqrt(L1_gpufit_Ex*L1_gpufit_Ex + L1_gpufit_Ey*L1_gpufit_Ey);
        L1_gpufit_sumEt = (l1_gpufit_cont->energyT())/1000.;
        L1_gpufit_Ex_log = signed_log(L1_gpufit_Ex, epsilon);
        L1_gpufit_Ey_log = signed_log(L1_gpufit_Ey, epsilon);
        L1_gpufit_Et_log = signed_log(L1_gpufit_Et, epsilon);
        L1_gpufit_sumEt_log = signed_log(L1_gpufit_sumEt, epsilon);
      }
    }


    // access HLT cell MET values
    if ( hlt_cell_met_cont.isValid() && hlt_cell_met_cont->size() > 0 ) {
      hlt_met = hlt_cell_met_cont->at(0);
      cell_Ex = (hlt_met->ex())/1000.;
      cell_Ey = (hlt_met->ey())/1000.;
      float cell_Ez = (hlt_met->ez())/1000.;
      cell_Et = std::sqrt(cell_Ex*cell_Ex + cell_Ey*cell_Ey);
      cell_presel_Et = std::sqrt(cell_Ex*cell_Ex + cell_Ey*cell_Ey);
      cell_sumEt = (hlt_met->sumEt())/1000.;
      cell_Ex_log = signed_log(cell_Ex, epsilon);
      cell_Ey_log = signed_log(cell_Ey, epsilon);
      cell_Et_log = signed_log(cell_Et, epsilon);
      cell_sumEt_log = signed_log(cell_sumEt, epsilon);

      TVector3 v(cell_Ex, cell_Ey, cell_Ez);
      cell_eta = v.Eta();
      cell_phi = v.Phi();

      // status, component
      std::vector<std::string> compNames = {
            "PreSamplB", "EMB1", "EMB2", "EMB3",  // LAr barrel
            "PreSamplE", "EME1", "EME2", "EME3",  // LAr EM endcap
            "HEC0",      "HEC1", "HEC2", "HEC3",  // Hadronic end cap cal.
            "TileBar0", "TileBar1", "TileBar2",   // Tile barrel
            "TileGap1", "TileGap2", "TileGap3",   // Tile gap (ITC & scint)
            "TileExt0", "TileExt1", "TileExt2",   // Tile extended barrel
            "FCalEM",   "FCalHad2", "FCalHad3",   // Forward cal endcap
            "Muons"
     };

     std::vector<std::string> bitNames = {
             "Processing",         // bit  0
             "ErrBSconv",          // bit  1
             "ErrMuon",            // bit  2
             "ErrFEB",             // bit  3
             "Skipped",            // bit  4
             "CompBigMEtSEtRatio", // bit  5
             "BadCompEnergy",      // bit  6
             "BadEnergyRatio",     // bit  7
             "spare",              // bit  8
             "BadCellQuality",     // bit  9
             "BadCellEnergy",      // bit 10
             "BadCellTime",        // bit 11
             "NoMuonTrack",        // bit 12
             "spare",              // bit 13
             "Processed",          // bit 14
             "CompError",          // bit 15
             "EMB_A_Missing",      // bit 16
             "EMB_C_Missing",      // bit 17
             "EME_A_Missing",      // bit 18
             "EME_C_Missing",      // bit 19
             "HEC_A_Missing",      // bit 20
             "HEC_C_Missing",      // bit 21
             "FCAL_A_Missing",     // bit 22
             "FCAL_C_Missing",     // bit 23
             "TileB_A_Missing",    // bit 24
             "TileB_C_Missing",    // bit 25
             "TileE_A_Missing",    // bit 26
             "TileE_C_Missing",    // bit 27
             "BadEMfraction",      // bit 28
             "GlobBigMEtSEtRatio", // bit 29
             "ObjInCrack",         // bit 30
             "GlobError"           // bit 31
      };

     for (int j=0; j<32; ++j) { //status loop
       unsigned mask = (1u<<j);
       if (hlt_met->flag() & mask) {
         MET_status = 1.;
       } else {
         MET_status = 0;
       }
       auto mon1 = Monitored::Scalar<std::string>( "HLT_MET_status",bitNames[j]);
       fill(tool,mon1,MET_status);
     }

     for (int i=0; i<24; ++i) { //component loop
       float ex = hlt_met->exComponent(i)*0.001;
       float ey = hlt_met->eyComponent(i)*0.001;
       component_Et = sqrt(ex*ex+ey*ey);
       auto mon2 = Monitored::Scalar<std::string>( "HLT_MET_component",compNames[i]);
       fill(tool,mon2,component_Et);
     }

     for (int i=0; i<24; ++i) { //component loop
       for (int j=0; j<32; ++j) { //status loop
         unsigned mask = (1u<<j);
         if (hlt_met->statusComponent(i) & mask) {
           component_status_weight = 1.;
         } else {
           component_status_weight = 0;
         }
         auto mon_bit = Monitored::Scalar<std::string>( "component_status",bitNames[j]);
         auto mon_comp = Monitored::Scalar<std::string>( "component",compNames[i]);
         fill(tool,mon_comp,mon_bit,component_status_weight);
       }
     }


    }
    ATH_MSG_DEBUG("cell_Et = " << cell_Et);

    // access HLT mht MET values
    if ( hlt_mht_met_cont.isValid() && hlt_mht_met_cont->size() > 0 ) {
      hlt_met = hlt_mht_met_cont->at(0);
      mht_Ex = (hlt_met->ex())/1000.;
      mht_Ey = (hlt_met->ey())/1000.;
      float mht_Ez = (hlt_met->ez())/1000.;
      mht_Et = std::sqrt(mht_Ex*mht_Ex + mht_Ey*mht_Ey);
      mht_presel_Et = std::sqrt(mht_Ex*mht_Ex + mht_Ey*mht_Ey);
      mht_sumEt = (hlt_met->sumEt())/1000.;
      mht_Ex_log = signed_log(mht_Ex, epsilon);
      mht_Ey_log = signed_log(mht_Ey, epsilon);
      mht_Et_log = signed_log(mht_Et, epsilon);
      mht_sumEt_log = signed_log(mht_sumEt, epsilon);

      TVector3 v(mht_Ex, mht_Ey, mht_Ez);
      mht_eta = v.Eta();
      mht_phi = v.Phi();
    }
    ATH_MSG_DEBUG("mht_Et = " << mht_Et);

    // access HLT tc_em MET values
    if ( hlt_tc_em_met_cont.isValid() && hlt_tc_em_met_cont->size() > 0 ) {
      hlt_met = hlt_tc_em_met_cont->at(0);
      tc_em_Ex = (hlt_met->ex())/1000.;
      tc_em_Ey = (hlt_met->ey())/1000.;
      float tc_em_Ez = (hlt_met->ez())/1000.;
      tc_em_Et = std::sqrt(tc_em_Ex*tc_em_Ex + tc_em_Ey*tc_em_Ey);
      tc_em_presel_Et = std::sqrt(tc_em_Ex*tc_em_Ex + tc_em_Ey*tc_em_Ey);
      tc_em_sumEt = (hlt_met->sumEt())/1000.;
      tc_em_Ex_log = signed_log(tc_em_Ex, epsilon);
      tc_em_Ey_log = signed_log(tc_em_Ey, epsilon);
      tc_em_Et_log = signed_log(tc_em_Et, epsilon);
      tc_em_sumEt_log = signed_log(tc_em_sumEt, epsilon);

      TVector3 v(tc_em_Ex, tc_em_Ey, tc_em_Ez);
      tc_em_eta = v.Eta();
      tc_em_phi = v.Phi();
    }
    ATH_MSG_DEBUG("tc_em_Et = " << tc_em_Et);

    // access HLT tclcw MET values
    if ( hlt_tc_met_cont.isValid() && hlt_tc_met_cont->size() > 0 ) {
      hlt_met = hlt_tc_met_cont->at(0);
      tc_Ex = (hlt_met->ex())/1000.;
      tc_Ey = (hlt_met->ey())/1000.;
      tc_Et = std::sqrt(tc_Ex*tc_Ex + tc_Ey*tc_Ey);
    }
    ATH_MSG_DEBUG("tc_Et = " << tc_Et);

    // access HLT trkmht MET values
    if ( hlt_trkmht_met_cont.isValid() && hlt_trkmht_met_cont->size() > 0 ) {
      hlt_met = hlt_trkmht_met_cont->at(0);
      trkmht_Ex = (hlt_met->ex())/1000.;
      trkmht_Ey = (hlt_met->ey())/1000.;
      float trkmht_Ez = (hlt_met->ez())/1000.;
      trkmht_Et = std::sqrt(trkmht_Ex*trkmht_Ex + trkmht_Ey*trkmht_Ey);
      trkmht_presel_Et = std::sqrt(trkmht_Ex*trkmht_Ex + trkmht_Ey*trkmht_Ey);
      trkmht_sumEt = (hlt_met->sumEt())/1000.;
      trkmht_Ex_log = signed_log(trkmht_Ex, epsilon);
      trkmht_Ey_log = signed_log(trkmht_Ey, epsilon);
      trkmht_Et_log = signed_log(trkmht_Et, epsilon);
      trkmht_sumEt_log = signed_log(trkmht_sumEt, epsilon);

      TVector3 v(trkmht_Ex, trkmht_Ey, trkmht_Ez);
      trkmht_phi = v.Phi();
    }
    ATH_MSG_DEBUG("trkmht_Et = " << trkmht_Et);

    // access HLT tcpufit MET values
    if ( hlt_tcpufit_met_cont.isValid() && hlt_tcpufit_met_cont->size() > 0 ) {
      hlt_met = hlt_tcpufit_met_cont->at(0);
      tcpufit_Ex = (hlt_met->ex())/1000.;
      tcpufit_Ey = (hlt_met->ey())/1000.;
      tcpufit_Ez = (hlt_met->ez())/1000.;
      tcpufit_Et = std::sqrt(tcpufit_Ex*tcpufit_Ex + tcpufit_Ey*tcpufit_Ey);
      tcpufit_presel_Et = std::sqrt(tcpufit_Ex*tcpufit_Ex + tcpufit_Ey*tcpufit_Ey);
      tcpufit_sumEt = (hlt_met->sumEt())/1000.;
      tcpufit_sumE = (hlt_met->sumE())/1000.;
      tcpufit_Ex_log = signed_log(tcpufit_Ex, epsilon);
      tcpufit_Ey_log = signed_log(tcpufit_Ey, epsilon);
      tcpufit_Ez_log = signed_log(tcpufit_Ez, epsilon);
      tcpufit_Et_log = signed_log(tcpufit_Et, epsilon);
      tcpufit_sumEt_log = signed_log(tcpufit_sumEt, epsilon);
      tcpufit_sumE_log = signed_log(tcpufit_sumE, epsilon);

      TVector3 v(tcpufit_Ex, tcpufit_Ey, tcpufit_Ez);
      tcpufit_eta = v.Eta();
      tcpufit_phi = v.Phi();
    }
    ATH_MSG_DEBUG("tcpufit_Et = " << tcpufit_Et);

    // access HLT pfsum MET values
    if ( hlt_pfsum_met_cont.isValid() && hlt_pfsum_met_cont->size() > 0 ) {
      hlt_met = hlt_pfsum_met_cont->at(0);
      pfsum_Ex = (hlt_met->ex())/1000.;
      pfsum_Ey = (hlt_met->ey())/1000.;
      float pfsum_Ez = (hlt_met->ez())/1000.;
      pfsum_Et = std::sqrt(pfsum_Ex*pfsum_Ex + pfsum_Ey*pfsum_Ey);
      pfsum_presel_Et = std::sqrt(pfsum_Ex*pfsum_Ex + pfsum_Ey*pfsum_Ey);
      pfsum_sumEt = (hlt_met->sumEt())/1000.;
      pfsum_Ex_log = signed_log(pfsum_Ex, epsilon);
      pfsum_Ey_log = signed_log(pfsum_Ey, epsilon);
      pfsum_Et_log = signed_log(pfsum_Et, epsilon);
      pfsum_sumEt_log = signed_log(pfsum_sumEt, epsilon);

      TVector3 v(pfsum_Ex, pfsum_Ey, pfsum_Ez);
      pfsum_phi = v.Phi();
    }
    ATH_MSG_DEBUG("pfsum_Et = " <<pfsum_Et);

    // access HLT pfsum_cssk MET values
    if ( hlt_pfsum_cssk_met_cont.isValid() && hlt_pfsum_cssk_met_cont->size() > 0 ) {
      hlt_met = hlt_pfsum_cssk_met_cont->at(0);
      pfsum_cssk_Ex = (hlt_met->ex())/1000.;
      pfsum_cssk_Ey = (hlt_met->ey())/1000.;
      float pfsum_cssk_Ez = (hlt_met->ez())/1000.;
      pfsum_cssk_Et = std::sqrt(pfsum_cssk_Ex*pfsum_cssk_Ex + pfsum_cssk_Ey*pfsum_cssk_Ey);
      pfsum_cssk_presel_Et = std::sqrt(pfsum_cssk_Ex*pfsum_cssk_Ex + pfsum_cssk_Ey*pfsum_cssk_Ey);
      pfsum_cssk_sumEt = (hlt_met->sumEt())/1000.;
      pfsum_cssk_Ex_log = signed_log(pfsum_cssk_Ex, epsilon);
      pfsum_cssk_Ey_log = signed_log(pfsum_cssk_Ey, epsilon);
      pfsum_cssk_Et_log = signed_log(pfsum_cssk_Et, epsilon);
      pfsum_cssk_sumEt_log = signed_log(pfsum_cssk_sumEt, epsilon);

      TVector3 v(pfsum_cssk_Ex, pfsum_cssk_Ey, pfsum_cssk_Ez);
      pfsum_cssk_phi = v.Phi();
    }
    ATH_MSG_DEBUG("pfsum_cssk_Et = " <<pfsum_cssk_Et);

    // access HLT pfsum_vssk MET values
    if ( hlt_pfsum_vssk_met_cont.isValid() && hlt_pfsum_vssk_met_cont->size() > 0 ) {
      hlt_met = hlt_pfsum_vssk_met_cont->at(0);
      pfsum_vssk_Ex = (hlt_met->ex())/1000.;
      pfsum_vssk_Ey = (hlt_met->ey())/1000.;
      float pfsum_vssk_Ez = (hlt_met->ez())/1000.;
      pfsum_vssk_Et = std::sqrt(pfsum_vssk_Ex*pfsum_vssk_Ex + pfsum_vssk_Ey*pfsum_vssk_Ey);
      pfsum_vssk_presel_Et = std::sqrt(pfsum_vssk_Ex*pfsum_vssk_Ex + pfsum_vssk_Ey*pfsum_vssk_Ey);
      pfsum_vssk_sumEt = (hlt_met->sumEt())/1000.;
      pfsum_vssk_Ex_log = signed_log(pfsum_vssk_Ex, epsilon);
      pfsum_vssk_Ey_log = signed_log(pfsum_vssk_Ey, epsilon);
      pfsum_vssk_Et_log = signed_log(pfsum_vssk_Et, epsilon);
      pfsum_vssk_sumEt_log = signed_log(pfsum_vssk_sumEt, epsilon);

      TVector3 v(pfsum_vssk_Ex, pfsum_vssk_Ey, pfsum_vssk_Ez);
      pfsum_vssk_phi = v.Phi();
    }
    ATH_MSG_DEBUG("pfsum_vssk_Et = " <<pfsum_vssk_Et);

    // acccess HLT pfopufit MET values
    if ( hlt_pfopufit_met_cont.isValid() && hlt_pfopufit_met_cont->size() > 0 ) {
      hlt_met = hlt_pfopufit_met_cont->at(0);
      pfopufit_Ex = (hlt_met->ex())/1000.;
      pfopufit_Ey = (hlt_met->ey())/1000.;
      float pfopufit_Ez = (hlt_met->ez())/1000.;
      pfopufit_Et = std::sqrt(pfopufit_Ex*pfopufit_Ex + pfopufit_Ey*pfopufit_Ey);
      pfopufit_presel_Et = std::sqrt(pfopufit_Ex*pfopufit_Ex + pfopufit_Ey*pfopufit_Ey);
      pfopufit_sumEt = (hlt_met->sumEt())/1000.;
      pfopufit_Ex_log = signed_log(pfopufit_Ex, epsilon);
      pfopufit_Ey_log = signed_log(pfopufit_Ey, epsilon);
      pfopufit_Et_log = signed_log(pfopufit_Et, epsilon);
      pfopufit_sumEt_log = signed_log(pfopufit_sumEt, epsilon);

      TVector3 v(pfopufit_Ex, pfopufit_Ey, pfopufit_Ez);
      pfopufit_phi = v.Phi();
      pfopufit_eta = v.Eta();
    }
    ATH_MSG_DEBUG("pfopufit_Et = " << pfopufit_Et);

    // access HLT cvfpufit MET values
    if ( hlt_cvfpufit_met_cont.isValid() && hlt_cvfpufit_met_cont->size() > 0 ) {
      hlt_met = hlt_cvfpufit_met_cont->at(0);
      cvfpufit_Ex = (hlt_met->ex())/1000.;
      cvfpufit_Ey = (hlt_met->ey())/1000.;
      float cvfpufit_Ez = (hlt_met->ez())/1000.;
      cvfpufit_Et = std::sqrt(cvfpufit_Ex*cvfpufit_Ex + cvfpufit_Ey*cvfpufit_Ey);
      cvfpufit_presel_Et = std::sqrt(cvfpufit_Ex*cvfpufit_Ex + cvfpufit_Ey*cvfpufit_Ey);
      cvfpufit_sumEt = (hlt_met->sumEt())/1000.;
      cvfpufit_Ex_log = signed_log(cvfpufit_Ex, epsilon);
      cvfpufit_Ey_log = signed_log(cvfpufit_Ey, epsilon);
      cvfpufit_Et_log = signed_log(cvfpufit_Et, epsilon);
      cvfpufit_sumEt_log = signed_log(cvfpufit_sumEt, epsilon);

      TVector3 v(cvfpufit_Ex, cvfpufit_Ey, cvfpufit_Ez);
      cvfpufit_phi = v.Phi();
    }
    ATH_MSG_DEBUG("cvfpufit_Et = " << cvfpufit_Et);

    // access HLT mhtpufit_pf MET values
    if ( hlt_mhtpufit_pf_met_cont.isValid() && hlt_mhtpufit_pf_met_cont->size() > 0 ) {
      hlt_met = hlt_mhtpufit_pf_met_cont->at(0);
      mhtpufit_pf_Ex = (hlt_met->ex())/1000.;
      mhtpufit_pf_Ey = (hlt_met->ey())/1000.;
      float mhtpufit_pf_Ez = (hlt_met->ez())/1000.;
      mhtpufit_pf_Et = std::sqrt(mhtpufit_pf_Ex*mhtpufit_pf_Ex + mhtpufit_pf_Ey*mhtpufit_pf_Ey);
      mhtpufit_pf_presel_Et = std::sqrt(mhtpufit_pf_Ex*mhtpufit_pf_Ex + mhtpufit_pf_Ey*mhtpufit_pf_Ey);
      mhtpufit_pf_sumEt = (hlt_met->sumEt())/1000.;
      mhtpufit_pf_Ex_log = signed_log(mhtpufit_pf_Ex, epsilon);
      mhtpufit_pf_Ey_log = signed_log(mhtpufit_pf_Ey, epsilon);
      mhtpufit_pf_Et_log = signed_log(mhtpufit_pf_Et, epsilon);
      mhtpufit_pf_sumEt_log = signed_log(mhtpufit_pf_sumEt, epsilon);

      TVector3 v(mhtpufit_pf_Ex, mhtpufit_pf_Ey, mhtpufit_pf_Ez);
      mhtpufit_pf_phi = v.Phi();
    }
    ATH_MSG_DEBUG("mhtpufit_pf_Et = " << mhtpufit_pf_Et);

    // access HLT mhtpufit_em MET values
    if ( hlt_mhtpufit_em_met_cont.isValid() && hlt_mhtpufit_em_met_cont->size() > 0 ) {
      hlt_met = hlt_mhtpufit_em_met_cont->at(0);
      mhtpufit_em_Ex = (hlt_met->ex())/1000.;
      mhtpufit_em_Ey = (hlt_met->ey())/1000.;
      float mhtpufit_em_Ez = (hlt_met->ez())/1000.;
      mhtpufit_em_Et = std::sqrt(mhtpufit_em_Ex*mhtpufit_em_Ex + mhtpufit_em_Ey*mhtpufit_em_Ey);
      mhtpufit_em_presel_Et = std::sqrt(mhtpufit_em_Ex*mhtpufit_em_Ex + mhtpufit_em_Ey*mhtpufit_em_Ey);
      mhtpufit_em_sumEt = (hlt_met->sumEt())/1000.;
      mhtpufit_em_Ex_log = signed_log(mhtpufit_em_Ex, epsilon);
      mhtpufit_em_Ey_log = signed_log(mhtpufit_em_Ey, epsilon);
      mhtpufit_em_Et_log = signed_log(mhtpufit_em_Et, epsilon);
      mhtpufit_em_sumEt_log = signed_log(mhtpufit_em_sumEt, epsilon);

      TVector3 v(mhtpufit_em_Ex, mhtpufit_em_Ey, mhtpufit_em_Ez);
      mhtpufit_em_phi = v.Phi();
    }
    ATH_MSG_DEBUG("mhtpufit_em_Et = " << mhtpufit_em_Et);

    // efficiency plots
    for (const std::string& chain : m_l1Chains) {
      auto pass_chain = Monitored::Scalar<float>("pass_" + chain, static_cast<float>(getTrigDecisionTool()->isPassed(chain)));
      fill(tool, pass_chain,offline_Et_eff);
    }
    for (const std::string& chain : m_hltChains) {
      auto pass_chain = Monitored::Scalar<float>("pass_" + chain, static_cast<float>(getTrigDecisionTool()->isPassed(chain)));
      fill(tool, pass_chain,offline_Et_eff);
    }

    // Fill. First argument is the tool (GMT) name as defined in the py file,
    // all others are the variables to be saved.
    //fill("TrigMETMonitor",L1_Ex,L1_Ey,L1_Et,pass_HLT1);
    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    fill(tool,offline_Ex,offline_Ey,offline_Et,offline_sumEt,
         L1_roi_Ex,L1_roi_Ey,L1_roi_Et,L1_roi_sumEt,
         L1_roi_Ex_log,L1_roi_Ey_log,L1_roi_Et_log,L1_roi_sumEt_log,
         L1_jnc_Ex,L1_jnc_Ey,L1_jnc_Et,L1_jnc_sumEt,
         L1_jnc_Ex_log,L1_jnc_Ey_log,L1_jnc_Et_log,L1_jnc_sumEt_log,
         L1_jrho_Ex,L1_jrho_Ey,L1_jrho_Et,L1_jrho_sumEt,
         L1_jrho_Ex_log,L1_jrho_Ey_log,L1_jrho_Et_log,L1_jrho_sumEt_log,
         L1_gnc_Ex,L1_gnc_Ey,L1_gnc_Et,L1_gnc_sumEt,
         L1_gnc_Ex_log,L1_gnc_Ey_log,L1_gnc_Et_log,L1_gnc_sumEt_log,
         L1_grho_Ex,L1_grho_Ey,L1_grho_Et,L1_grho_sumEt,
         L1_grho_Ex_log,L1_grho_Ey_log,L1_grho_Et_log,L1_grho_sumEt_log,
         L1_gjwoj_Ex,L1_gjwoj_Ey,L1_gjwoj_Et,L1_gjwoj_sumEt,
         L1_gjwoj_Ex_log,L1_gjwoj_Ey_log,L1_gjwoj_Et_log,L1_gjwoj_sumEt_log,
         L1_gpufit_Ex,L1_gpufit_Ey,L1_gpufit_Et,L1_gpufit_sumEt,
         L1_gpufit_Ex_log,L1_gpufit_Ey_log,L1_gpufit_Et_log,L1_gpufit_sumEt_log);
    if (hlt_el_mult > 0) {
      fill(tool,hlt_el_mult);
    }
    if (hlt_mu_mult > 0) {
      fill(tool,hlt_mu_mult);
    }
    if (hlt_cell_met_cont->size() > 0) {
      fill(tool,cell_Ex,cell_Ey,cell_Et,cell_sumEt,
         cell_Ex_log,cell_Ey_log,cell_Et_log,cell_sumEt_log,
         cell_eta,cell_phi,
         mht_Ex,mht_Ey,mht_Et,mht_sumEt,
         mht_Ex_log,mht_Ey_log,mht_Et_log,mht_sumEt_log,
         mht_eta,mht_phi,
         tc_em_Ex,tc_em_Ey,tc_em_Et,tc_em_sumEt,
         tc_em_Ex_log,tc_em_Ey_log,tc_em_Et_log,tc_em_sumEt_log,
         tc_em_eta,tc_em_phi,
         tc_Ex,tc_Ey,tc_Et,
         trkmht_Ex,trkmht_Ey,trkmht_Et,trkmht_sumEt,
         trkmht_Ex_log,trkmht_Ey_log,trkmht_Et_log,trkmht_sumEt_log,
         trkmht_phi,
         pfsum_Ex,pfsum_Ey,pfsum_Et,pfsum_sumEt,
         pfsum_Ex_log,pfsum_Ey_log,pfsum_Et_log,pfsum_sumEt_log,
         pfsum_phi,
         pfsum_cssk_Ex,pfsum_cssk_Ey,pfsum_cssk_Et,pfsum_cssk_sumEt,
         pfsum_cssk_Ex_log,pfsum_cssk_Ey_log,pfsum_cssk_Et_log,pfsum_cssk_sumEt_log,
         pfsum_cssk_phi,
         pfsum_vssk_Ex,pfsum_vssk_Ey,pfsum_vssk_Et,pfsum_vssk_sumEt,
         pfsum_vssk_Ex_log,pfsum_vssk_Ey_log,pfsum_vssk_Et_log,pfsum_vssk_sumEt_log,
         pfsum_vssk_phi,
         pfopufit_Ex,pfopufit_Ey,pfopufit_Et,pfopufit_sumEt,
         pfopufit_Ex_log,pfopufit_Ey_log,pfopufit_Et_log,pfopufit_sumEt_log,
         pfopufit_phi,pfopufit_eta,
         cvfpufit_Ex,cvfpufit_Ey,cvfpufit_Et,cvfpufit_sumEt,
         cvfpufit_Ex_log,cvfpufit_Ey_log,cvfpufit_Et_log,cvfpufit_sumEt_log,
         cvfpufit_phi,
         mhtpufit_pf_Ex,mhtpufit_pf_Ey,mhtpufit_pf_Et,mhtpufit_pf_sumEt,
         mhtpufit_pf_Ex_log,mhtpufit_pf_Ey_log,mhtpufit_pf_Et_log,mhtpufit_pf_sumEt_log,
         mhtpufit_pf_phi,
         mhtpufit_em_Ex,mhtpufit_em_Ey,mhtpufit_em_Et,mhtpufit_em_sumEt,
         mhtpufit_em_Ex_log,mhtpufit_em_Ey_log,mhtpufit_em_Et_log,mhtpufit_em_sumEt_log,
         mhtpufit_em_phi,
         tcpufit_Ex,tcpufit_Ey,tcpufit_Ez,tcpufit_Et,tcpufit_sumEt,tcpufit_sumE,
         tcpufit_Ex_log,tcpufit_Ey_log,tcpufit_Ez_log,tcpufit_Et_log,tcpufit_sumEt_log,tcpufit_sumE_log,
         tcpufit_eta,tcpufit_phi);
    }

    if(L1_roi_Et > 50.){
      fill(tool,
      cell_presel_Et,
      tcpufit_presel_Et,
      trkmht_presel_Et,
      mht_presel_Et,
      tc_em_presel_Et,
      pfsum_presel_Et,
      pfsum_cssk_presel_Et,
      pfsum_vssk_presel_Et,
      pfopufit_presel_Et,
      cvfpufit_presel_Et,
      mhtpufit_pf_presel_Et,
      mhtpufit_em_presel_Et);
    }

    return StatusCode::SUCCESS;
}


double TrigMETMonitorAlgorithm::signed_log(double e, double epsilon) const {


  double e_log = -9e9;
  if (std::abs(e) > epsilon)
    e_log = std::copysign(std::log10(std::abs(e)), e);
  else
    e_log = 0.01;

  return e_log;
}
