/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitorAlgorithm.h"
#include <TVector3.h>

TrigMETMonitorAlgorithm::TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  , m_offline_met_key("MET_Reference_AntiKt4EMPFlow")
  , m_hlt_electron_key("HLT_egamma_Electrons_GSF")
  , m_hlt_muon_key("HLT_MuonsCB_FS")
  , m_lvl1_roi_key("LVL1EnergySumRoI")
  , m_lvl1_jnc_key("jNOISECUT_MET")
  , m_lvl1_jrho_key("jXERHO_MET")
  , m_lvl1_gnc_key("gXENOISECUT_MET")
  , m_lvl1_grho_key("gXERHO_MET")
  , m_lvl1_gjwoj_key("gXEJWOJ_MET")
  , m_lvl1_gpufit_key("gXEPUFIT_MET")
  , m_hlt_cell_met_key("HLT_MET_cell")
  , m_hlt_mht_met_key("HLT_MET_mht")
  , m_hlt_tc_met_key("HLT_MET_tc")
  , m_hlt_tc_em_met_key("HLT_MET_tc_em")
  , m_hlt_tcpufit_met_key("HLT_MET_tcpufit")
  , m_hlt_trkmht_met_key("HLT_MET_trkmht")
  , m_hlt_pfsum_met_key("HLT_MET_pfsum")
  , m_hlt_pfsum_cssk_met_key("HLT_MET_pfsum_cssk")
  , m_hlt_pfsum_vssk_met_key("HLT_MET_pfsum_vssk")
  , m_hlt_pfopufit_met_key("HLT_MET_pfopufit")
  , m_hlt_cvfpufit_met_key("HLT_MET_cvfpufit")
  , m_hlt_mhtpufit_pf_met_key("HLT_MET_mhtpufit_pf_subjesgscIS")
  , m_hlt_mhtpufit_em_met_key("HLT_MET_mhtpufit_em_subjesgscIS")
  , m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty("offline_met_key", m_offline_met_key);
  declareProperty("hlt_electron_key", m_hlt_electron_key);
  declareProperty("hlt_muon_key", m_hlt_muon_key);
  declareProperty("l1_roi_key", m_lvl1_roi_key);
  declareProperty("l1_jnc_key", m_lvl1_jnc_key);
  declareProperty("l1_jrho_key", m_lvl1_jrho_key);
  declareProperty("l1_gnc_key", m_lvl1_gnc_key);
  declareProperty("l1_grho_key", m_lvl1_grho_key);
  declareProperty("l1_gjwoj_key", m_lvl1_gjwoj_key);
  declareProperty("l1_gpufit_key", m_lvl1_gpufit_key);
  declareProperty("hlt_cell_key", m_hlt_cell_met_key);
  declareProperty("hlt_mht_key", m_hlt_mht_met_key);
  declareProperty("hlt_tc_key", m_hlt_tc_met_key);
  declareProperty("hlt_tc_em_key", m_hlt_tc_em_met_key);
  declareProperty("hlt_tcpufit_key", m_hlt_tcpufit_met_key);
  declareProperty("hlt_trkmht_key", m_hlt_trkmht_met_key);
  declareProperty("hlt_pfsum_key", m_hlt_pfsum_met_key);
  declareProperty("hlt_pfsum_cssk_key", m_hlt_pfsum_cssk_met_key);
  declareProperty("hlt_pfsum_vssk_key", m_hlt_pfsum_vssk_met_key);
  declareProperty("hlt_pfopufit_key", m_hlt_pfopufit_met_key);
  declareProperty("hlt_cvfpufit_key", m_hlt_cvfpufit_met_key);
  declareProperty("hlt_mhtpufit_pf_key", m_hlt_mhtpufit_pf_met_key);
  declareProperty("hlt_mhtpufit_em_key", m_hlt_mhtpufit_em_met_key);

  declareProperty("L1Chain01", m_L1Chain01="L1_XE50");
  declareProperty("L1Chain02", m_L1Chain02="L1_jXENC50");
  declareProperty("L1Chain03", m_L1Chain03="L1_jXERHO50");
  declareProperty("L1Chain04", m_L1Chain04="L1_gXENC50");
  declareProperty("L1Chain05", m_L1Chain05="L1_gXERHO50");
  declareProperty("L1Chain06", m_L1Chain06="L1_gXEJWOJ50");
  declareProperty("L1Chain07", m_L1Chain07="L1_gXEPUFIT50");
  declareProperty("HLTChain01", m_HLTChain01="HLT_xe65_cell_L1XE50");
  declareProperty("HLTChain02", m_HLTChain02="HLT_xe100_mht_L1XE50");
  declareProperty("HLTChain03", m_HLTChain03="HLT_xe100_tcpufit_L1XE50");
  declareProperty("HLTChain04", m_HLTChain04="HLT_xe100_trkmht_L1XE50");
  declareProperty("HLTChain05", m_HLTChain05="HLT_xe100_pfsum_L1XE50");
  declareProperty("HLTChain06", m_HLTChain06="HLT_xe100_pfopuft_L1XE50");
  declareProperty("HLTChain07", m_HLTChain07="HLT_xe100_cvfpufit_L1XE50");
  declareProperty("HLTChain08", m_HLTChain08="HLT_xe100_mhtpufit_em_subjesgscIS_L1XE50");
  declareProperty("HLTChain09", m_HLTChain09="HLT_xe100_mhtpufit_pf_subjesgscIS_L1XE50");
  declareProperty("HLTChain10", m_HLTChain10="HLT_xe100_pfsum_cssk_L1XE50");
  declareProperty("HLTChain11", m_HLTChain11="HLT_xe100_pfsum_vssk_L1XE50");
  declareProperty("HLTChain12", m_HLTChain12="HLT_xe65_cell_xe110_tcpuft_L1XE50");
  declareProperty("HLTChain13", m_HLTChain13="HLT_xe100_trkmht_xe85_tcpufit_xe65_cell_L1XE50");
  declareProperty("HLTChain14", m_HLTChain14="HLT_xe95_trkmht_xe90_tcpufit_xe75_cell_L1XE50");
}


TrigMETMonitorAlgorithm::~TrigMETMonitorAlgorithm() {}


StatusCode TrigMETMonitorAlgorithm::initialize() {
    ATH_CHECK( m_offline_met_key.initialize() );
    ATH_CHECK( m_hlt_electron_key.initialize() );
    ATH_CHECK( m_hlt_muon_key.initialize() );
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

    ATH_CHECK( m_trigDecTool.retrieve() );

    return AthMonitorAlgorithm::initialize();
}


StatusCode TrigMETMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // access lepton containers
    SG::ReadHandle<xAOD::ElectronContainer> hlt_electron_cont(m_hlt_electron_key, ctx);
    if (! hlt_electron_cont.isValid() || hlt_electron_cont->size()==0 ) {
      ATH_MSG_DEBUG("Container "<< m_hlt_electron_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::MuonContainer> hlt_muon_cont(m_hlt_muon_key, ctx);
    if (! hlt_muon_cont.isValid() || hlt_muon_cont->size()==0 ) {
      ATH_MSG_DEBUG("Container "<< m_hlt_muon_key << " does not exist or is empty");
    }

    // access met containers
    SG::ReadHandle<xAOD::MissingETContainer> offline_met_cont(m_offline_met_key, ctx);
    if (! offline_met_cont.isValid() || offline_met_cont->size()==0 ) {
      ATH_MSG_DEBUG("Container "<< m_offline_met_key << " does not exist or is empty");
    }

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
    auto hlt_el_mult = Monitored::Scalar<int>("hlt_el_mult",0.0);
    auto hlt_el_pt = Monitored::Scalar<float>("hlt_el_pt",0.0);
    auto hlt_mu_mult = Monitored::Scalar<int>("hlt_mu_mult",0.0);
    auto hlt_mu_pt = Monitored::Scalar<float>("hlt_mu_pt",0.0);
    auto offline_Ex = Monitored::Scalar<float>("offline_Ex",0.0);
    auto offline_Ey = Monitored::Scalar<float>("offline_Ey",0.0);
    auto offline_Et = Monitored::Scalar<float>("offline_Et",0.0);
    auto offline_sumEt = Monitored::Scalar<float>("offline_sumEt",0.0);
    auto L1_roi_Ex = Monitored::Scalar<float>("L1_roi_Ex",0.0);
    auto L1_roi_Ey = Monitored::Scalar<float>("L1_roi_Ey",0.0);
    auto L1_roi_Et = Monitored::Scalar<float>("L1_roi_Et",0.0);
    auto L1_roi_sumEt = Monitored::Scalar<float>("L1_roi_sumEt",0.0);
    auto L1_jnc_Ex = Monitored::Scalar<float>("L1_jnc_Ex",0.0);
    auto L1_jnc_Ey = Monitored::Scalar<float>("L1_jnc_Ey",0.0);
    auto L1_jnc_Et = Monitored::Scalar<float>("L1_jnc_Et",0.0);
    auto L1_jnc_sumEt = Monitored::Scalar<float>("L1_jnc_sumEt",0.0);
    auto L1_jrho_Ex = Monitored::Scalar<float>("L1_jrho_Ex",0.0);
    auto L1_jrho_Ey = Monitored::Scalar<float>("L1_jrho_Ey",0.0);
    auto L1_jrho_Et = Monitored::Scalar<float>("L1_jrho_Et",0.0);
    auto L1_jrho_sumEt = Monitored::Scalar<float>("L1_jrho_sumEt",0.0);
    auto L1_gnc_Ex = Monitored::Scalar<float>("L1_gnc_Ex",0.0);
    auto L1_gnc_Ey = Monitored::Scalar<float>("L1_gnc_Ey",0.0);
    auto L1_gnc_Et = Monitored::Scalar<float>("L1_gnc_Et",0.0);
    auto L1_gnc_sumEt = Monitored::Scalar<float>("L1_gnc_sumEt",0.0);
    auto L1_grho_Ex = Monitored::Scalar<float>("L1_grho_Ex",0.0);
    auto L1_grho_Ey = Monitored::Scalar<float>("L1_grho_Ey",0.0);
    auto L1_grho_Et = Monitored::Scalar<float>("L1_grho_Et",0.0);
    auto L1_grho_sumEt = Monitored::Scalar<float>("L1_grho_sumEt",0.0);
    auto L1_gjwoj_Ex = Monitored::Scalar<float>("L1_gjwoj_Ex",0.0);
    auto L1_gjwoj_Ey = Monitored::Scalar<float>("L1_gjwoj_Ey",0.0);
    auto L1_gjwoj_Et = Monitored::Scalar<float>("L1_gjwoj_Et",0.0);
    auto L1_gjwoj_sumEt = Monitored::Scalar<float>("L1_gjwoj_sumEt",0.0);
    auto L1_gpufit_Ex = Monitored::Scalar<float>("L1_gpufit_Ex",0.0);
    auto L1_gpufit_Ey = Monitored::Scalar<float>("L1_gpufit_Ey",0.0);
    auto L1_gpufit_Et = Monitored::Scalar<float>("L1_gpufit_Et",0.0);
    auto L1_gpufit_sumEt = Monitored::Scalar<float>("L1_gpufit_sumEt",0.0);
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
    auto pass_L101 = Monitored::Scalar<float>("pass_L101",0.0);
    auto pass_L102 = Monitored::Scalar<float>("pass_L102",0.0);
    auto pass_L103 = Monitored::Scalar<float>("pass_L103",0.0);
    auto pass_L104 = Monitored::Scalar<float>("pass_L104",0.0);
    auto pass_L105 = Monitored::Scalar<float>("pass_L105",0.0);
    auto pass_L106 = Monitored::Scalar<float>("pass_L106",0.0);
    auto pass_L107 = Monitored::Scalar<float>("pass_L107",0.0);
    auto pass_HLT01 = Monitored::Scalar<float>("pass_HLT01",0.0);
    auto pass_HLT02 = Monitored::Scalar<float>("pass_HLT02",0.0);
    auto pass_HLT03 = Monitored::Scalar<float>("pass_HLT03",0.0);
    auto pass_HLT04 = Monitored::Scalar<float>("pass_HLT04",0.0);
    auto pass_HLT05 = Monitored::Scalar<float>("pass_HLT05",0.0);
    auto pass_HLT06 = Monitored::Scalar<float>("pass_HLT06",0.0);
    auto pass_HLT07 = Monitored::Scalar<float>("pass_HLT07",0.0);
    auto pass_HLT08 = Monitored::Scalar<float>("pass_HLT08",0.0);
    auto pass_HLT09 = Monitored::Scalar<float>("pass_HLT09",0.0);
    auto pass_HLT10 = Monitored::Scalar<float>("pass_HLT10",0.0);
    auto pass_HLT11 = Monitored::Scalar<float>("pass_HLT11",0.0);
    auto pass_HLT12 = Monitored::Scalar<float>("pass_HLT12",0.0);
    auto pass_HLT13 = Monitored::Scalar<float>("pass_HLT13",0.0);
    auto pass_HLT14 = Monitored::Scalar<float>("pass_HLT14",0.0);


    // constant floor for log plots
    double epsilon = 1.189;

    // access lepton values
    if ( hlt_electron_cont.isValid() && hlt_electron_cont->size() > 0 ) {
      hlt_el_mult = hlt_electron_cont->size();
      for (auto Electron: *hlt_electron_cont) {
        hlt_el_pt = (Electron->pt())*0.001;
      }
    }

    if ( hlt_muon_cont.isValid() && hlt_muon_cont->size() > 0 ) {
      hlt_mu_mult = hlt_muon_cont->size();
      for (auto Muon: *hlt_muon_cont) {
        hlt_mu_pt = (Muon->pt())*0.001;
      }
    }

    // access offline MET values
    if ( offline_met_cont.isValid() && offline_met_cont->size() > 0 ) {
      offline_met = offline_met_cont->at(0);
      offline_Ex = - (offline_met->mpx())/1000.;
      offline_Ey = - (offline_met->mpy())/1000.;
      offline_sumEt = (offline_met->sumet())/1000.;
      offline_Et = std::sqrt(offline_Ex*offline_Ex + offline_Ey*offline_Ey);
    }

    // access L1 roi MET values
    if ( l1_roi_cont.isValid() ) {
      if ((l1_roi_cont->energyX())>-9e12 && (l1_roi_cont->energyX())<9e12 && (l1_roi_cont->energyY())>-9e12 && (l1_roi_cont->energyY())<9e12) { 
	L1_roi_Ex = - (l1_roi_cont->energyX())/1000.;
	L1_roi_Ey = - (l1_roi_cont->energyY())/1000.;
        L1_roi_Et = std::sqrt(L1_roi_Ex*L1_roi_Ex + L1_roi_Ey*L1_roi_Ey);
        L1_roi_sumEt = (l1_roi_cont->energyT())/1000.;
      }
    }

    // access L1 jnc MET values
    if ( l1_jnc_cont.isValid() ) {
      if ((l1_jnc_cont->energyX())>-9e12 && (l1_jnc_cont->energyX())<9e12 && (l1_jnc_cont->energyY())>-9e12 && (l1_jnc_cont->energyY())<9e12) {
        L1_jnc_Ex = - (l1_jnc_cont->energyX())/1000.;
        L1_jnc_Ey = - (l1_jnc_cont->energyY())/1000.;
        L1_jnc_Et = std::sqrt(L1_jnc_Ex*L1_jnc_Ex + L1_jnc_Ey*L1_jnc_Ey);
        L1_jnc_sumEt = (l1_jnc_cont->energyT())/1000.;
      }
    }

    // access L1 jrho MET values
    if ( l1_jrho_cont.isValid() ) {
      if ((l1_jrho_cont->energyX())>-9e12 && (l1_jrho_cont->energyX())<9e12 && (l1_jrho_cont->energyY())>-9e12 && (l1_jrho_cont->energyY())<9e12) {
        L1_jrho_Ex = - (l1_jrho_cont->energyX())/1000.;
        L1_jrho_Ey = - (l1_jrho_cont->energyY())/1000.;
        L1_jrho_Et = std::sqrt(L1_jrho_Ex*L1_jrho_Ex + L1_jrho_Ey*L1_jrho_Ey);
        L1_jrho_sumEt = (l1_jrho_cont->energyT())/1000.;
      }
    }

    // access L1 gnc MET values
    if ( l1_gnc_cont.isValid() ) {
      if ((l1_gnc_cont->energyX())>-9e12 && (l1_gnc_cont->energyX())<9e12 && (l1_gnc_cont->energyY())>-9e12 && (l1_gnc_cont->energyY())<9e12) {
        L1_gnc_Ex = - (l1_gnc_cont->energyX())/1000.;
        L1_gnc_Ey = - (l1_gnc_cont->energyY())/1000.;
        L1_gnc_Et = std::sqrt(L1_gnc_Ex*L1_gnc_Ex + L1_gnc_Ey*L1_gnc_Ey);
        L1_gnc_sumEt = (l1_gnc_cont->energyT())/1000.;
      }
    }

    // access L1 grho MET values
    if ( l1_grho_cont.isValid() ) {
      if ((l1_grho_cont->energyX())>-9e12 && (l1_grho_cont->energyX())<9e12 && (l1_grho_cont->energyY())>-9e12 && (l1_grho_cont->energyY())<9e12) {
        L1_grho_Ex = - (l1_grho_cont->energyX())/1000.;
        L1_grho_Ey = - (l1_grho_cont->energyY())/1000.;
        L1_grho_Et = std::sqrt(L1_grho_Ex*L1_grho_Ex + L1_grho_Ey*L1_grho_Ey);
        L1_grho_sumEt = (l1_grho_cont->energyT())/1000.;
      }
    }

    // access L1 gjwoj MET values
    if ( l1_gjwoj_cont.isValid() ) {
      if ((l1_gjwoj_cont->energyX())>-9e12 && (l1_gjwoj_cont->energyX())<9e12 && (l1_gjwoj_cont->energyY())>-9e12 && (l1_gjwoj_cont->energyY())<9e12) {
        L1_gjwoj_Ex = - (l1_gjwoj_cont->energyX())/1000.;
        L1_gjwoj_Ey = - (l1_gjwoj_cont->energyY())/1000.;
        L1_gjwoj_Et = std::sqrt(L1_gjwoj_Ex*L1_gjwoj_Ex + L1_gjwoj_Ey*L1_gjwoj_Ey);
        L1_gjwoj_sumEt = (l1_gjwoj_cont->energyT())/1000.;
      }
    }

    // access L1 gpufit MET values
    if ( l1_gpufit_cont.isValid() ) {
      if ((l1_gpufit_cont->energyX())>-9e12 && (l1_gpufit_cont->energyX())<9e12 && (l1_gpufit_cont->energyY())>-9e12 && (l1_gpufit_cont->energyY())<9e12) {
        L1_gpufit_Ex = - (l1_gpufit_cont->energyX())/1000.;
        L1_gpufit_Ey = - (l1_gpufit_cont->energyY())/1000.;
        L1_gpufit_Et = std::sqrt(L1_gpufit_Ex*L1_gpufit_Ex + L1_gpufit_Ey*L1_gpufit_Ey);
        L1_gpufit_sumEt = (l1_gpufit_cont->energyT())/1000.;
      }
    }


    // access HLT cell MET values
    if ( hlt_cell_met_cont.isValid() && hlt_cell_met_cont->size() > 0 ) {
      hlt_met = hlt_cell_met_cont->at(0);
      cell_Ex = (hlt_met->ex())/1000.;
      cell_Ey = (hlt_met->ey())/1000.;
      float cell_Ez = (hlt_met->ez())/1000.;
      cell_Et = std::sqrt(cell_Ex*cell_Ex + cell_Ey*cell_Ey);
      cell_sumEt = (hlt_met->sumEt())/1000.;
      cell_Ex_log = signed_log(cell_Ex, epsilon);
      cell_Ey_log = signed_log(cell_Ey, epsilon);
      cell_Et_log = signed_log(cell_Et, epsilon);
      cell_sumEt_log = signed_log(cell_sumEt, epsilon);

      TVector3 v(cell_Ex, cell_Ey, cell_Ez);
      cell_eta = v.Eta();
      cell_phi = v.Phi();
    }
    ATH_MSG_DEBUG("cell_Et = " << cell_Et);

    // access HLT mht MET values
    if ( hlt_mht_met_cont.isValid() && hlt_mht_met_cont->size() > 0 ) {
      hlt_met = hlt_mht_met_cont->at(0);
      mht_Ex = (hlt_met->ex())/1000.;
      mht_Ey = (hlt_met->ey())/1000.;
      float mht_Ez = (hlt_met->ez())/1000.;
      mht_Et = std::sqrt(mht_Ex*mht_Ex + mht_Ey*mht_Ey);
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
      pfopufit_sumEt = (hlt_met->sumEt())/1000.;
      pfopufit_Ex_log = signed_log(pfopufit_Ex, epsilon);
      pfopufit_Ey_log = signed_log(pfopufit_Ey, epsilon);
      pfopufit_Et_log = signed_log(pfopufit_Et, epsilon);
      pfopufit_sumEt_log = signed_log(pfopufit_sumEt, epsilon);
    
      TVector3 v(pfopufit_Ex, pfopufit_Ey, pfopufit_Ez);
      pfopufit_phi = v.Phi();
    }
    ATH_MSG_DEBUG("pfopufit_Et = " << pfopufit_Et);

    // access HLT cvfpufit MET values
    if ( hlt_cvfpufit_met_cont.isValid() && hlt_cvfpufit_met_cont->size() > 0 ) {
      hlt_met = hlt_cvfpufit_met_cont->at(0);
      cvfpufit_Ex = (hlt_met->ex())/1000.;
      cvfpufit_Ey = (hlt_met->ey())/1000.;
      float cvfpufit_Ez = (hlt_met->ez())/1000.;
      cvfpufit_Et = std::sqrt(cvfpufit_Ex*cvfpufit_Ex + cvfpufit_Ey*cvfpufit_Ey);
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
    if (m_trigDecTool->isPassed(m_L1Chain01)) pass_L101 = 1.0;
    if (m_trigDecTool->isPassed(m_L1Chain02)) pass_L102 = 1.0;
    if (m_trigDecTool->isPassed(m_L1Chain03)) pass_L103 = 1.0;
    if (m_trigDecTool->isPassed(m_L1Chain04)) pass_L104 = 1.0;
    if (m_trigDecTool->isPassed(m_L1Chain05)) pass_L105 = 1.0;
    if (m_trigDecTool->isPassed(m_L1Chain06)) pass_L106 = 1.0;
    if (m_trigDecTool->isPassed(m_L1Chain07)) pass_L107 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain01)) pass_HLT01 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain02)) pass_HLT02 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain03)) pass_HLT03 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain04)) pass_HLT04 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain05)) pass_HLT05 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain06)) pass_HLT06 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain07)) pass_HLT07 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain08)) pass_HLT08 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain09)) pass_HLT09 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain10)) pass_HLT10 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain11)) pass_HLT11 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain12)) pass_HLT12 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain13)) pass_HLT13 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain14)) pass_HLT14 = 1.0;
    ATH_MSG_DEBUG("pass " << m_L1Chain01 << " = " << pass_L101);
    ATH_MSG_DEBUG("pass " << m_HLTChain01 << " = " << pass_HLT01);
    ATH_MSG_DEBUG("pass " << m_HLTChain02 << " = " << pass_HLT02);


    // Fill. First argument is the tool (GMT) name as defined in the py file, 
    // all others are the variables to be saved.
    //fill("TrigMETMonitor",L1_Ex,L1_Ey,L1_Et,pass_HLT1);
    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigMETMonitor");
    fill(tool,offline_Ex,offline_Ey,offline_Et,offline_sumEt,
         L1_roi_Ex,L1_roi_Ey,L1_roi_Et,L1_roi_sumEt,
         L1_jnc_Ex,L1_jnc_Ey,L1_jnc_Et,L1_jnc_sumEt,
         L1_jrho_Ex,L1_jrho_Ey,L1_jrho_Et,L1_jrho_sumEt,
         L1_gnc_Ex,L1_gnc_Ey,L1_gnc_Et,L1_gnc_sumEt,
         L1_grho_Ex,L1_grho_Ey,L1_grho_Et,L1_grho_sumEt,
         L1_gjwoj_Ex,L1_gjwoj_Ey,L1_gjwoj_Et,L1_gjwoj_sumEt,
         L1_gpufit_Ex,L1_gpufit_Ey,L1_gpufit_Et,L1_gpufit_sumEt,
         pass_L101,pass_L102,pass_L103,pass_L104,pass_L105,
         pass_L106,pass_L107,
         pass_HLT01,pass_HLT02,pass_HLT03,pass_HLT04,pass_HLT05,
         pass_HLT05,pass_HLT06,pass_HLT07,pass_HLT08,pass_HLT09,
         pass_HLT11,pass_HLT12,pass_HLT13,pass_HLT14);
    if (hlt_el_mult > 0) {
      fill(tool,hlt_el_mult, hlt_el_pt);
    }
    if (hlt_mu_mult > 0) {
      fill(tool,hlt_mu_mult, hlt_mu_pt);
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
         pfopufit_phi,
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

