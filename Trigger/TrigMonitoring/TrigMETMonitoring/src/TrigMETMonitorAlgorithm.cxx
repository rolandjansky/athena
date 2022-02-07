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
    // Fill. First argument is the tool (GMT) name as defined in the py file,
    // all others are the variables to be saved.
    auto tool = getGroup("TrigMETMonitor");

    // access pileup <mu>
    act_IPBC = eventInfo->actualInteractionsPerCrossing();
    fill(tool, act_IPBC);

    // access lepton values
    if ( hlt_electron_cont.isValid() && hlt_electron_cont->size() > 0 ) {
      hlt_el_mult = hlt_electron_cont->size();
      if (hlt_el_mult > 0) fill(tool,hlt_el_mult);
      for (auto Electron: *hlt_electron_cont) {
        hlt_el_pt = (Electron->pt())*0.001;
        if (hlt_el_pt > 0) {
          fill(tool, hlt_el_pt);
        }
      }
    }

    if( hlt_muon_cont.isValid() && hlt_muon_cont->size() > 0){
      hlt_mu_mult = hlt_muon_cont->size();
      if (hlt_mu_mult > 0) fill(tool,hlt_mu_mult);
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
    const xAOD::MissingET *offline_met = 0;
    if ( offline_met_cont.isValid() && offline_met_cont->size() > 0 ) {
      offline_met = offline_met_cont->at(0);
      offline_Ex = - (offline_met->mpx())/1000.;
      offline_Ey = - (offline_met->mpy())/1000.;
      offline_sumEt = (offline_met->sumet())/1000.;
      offline_Et = std::sqrt(offline_Ex*offline_Ex + offline_Ey*offline_Ey);
      offline_Et_eff = std::sqrt(offline_Ex*offline_Ex + offline_Ey*offline_Ey);
    }
    fill(tool,offline_Ex,offline_Ey,offline_Et,offline_sumEt);

    
    // access L1 MET values
    SG::ReadHandle<xAOD::EnergySumRoI> l1_met_cont;
    for (const std::string& alg : m_algsL1) {
      if (alg == "roi" && l1_roi_cont.isValid()) {
        l1_met_cont = l1_roi_cont;
      } else if (alg == "jnc" && l1_jnc_cont.isValid()) {
        l1_met_cont = l1_jnc_cont;
      } else if (alg == "jrho" && l1_jrho_cont.isValid()) {
        l1_met_cont = l1_jrho_cont;
      } else if (alg == "gnc" && l1_gnc_cont.isValid()) {
        l1_met_cont = l1_gnc_cont;
      } else if (alg == "grho" && l1_grho_cont.isValid()) {
        l1_met_cont = l1_grho_cont;
      } else if (alg == "gjwoj" && l1_gjwoj_cont.isValid()) {
        l1_met_cont = l1_gjwoj_cont;
      } else if (alg == "gpufit" && l1_gpufit_cont.isValid()) {
        l1_met_cont = l1_gpufit_cont;
      }

      if ( l1_met_cont.isValid() ) {
        if ((l1_met_cont->energyX())>-9e12 && (l1_met_cont->energyX())<9e12 && (l1_met_cont->energyY())>-9e12 && (l1_met_cont->energyY())<9e12) {
          float L1_met_Ex = - (l1_met_cont->energyX())/1000.;
          float L1_met_Ey = - (l1_met_cont->energyY())/1000.;
          float L1_met_Et = std::sqrt(L1_met_Ex*L1_met_Ex + L1_met_Ey*L1_met_Ey);
          float L1_met_sumEt = (l1_met_cont->energyT())/1000.;
          float L1_met_Ex_log = signed_log(L1_met_Ex, epsilon);
          float L1_met_Ey_log = signed_log(L1_met_Ey, epsilon);
          float L1_met_Et_log = signed_log(L1_met_Et, epsilon);
          float L1_met_sumEt_log = signed_log(L1_met_sumEt, epsilon);

          auto L1_Ex = Monitored::Scalar<float>("L1_"+alg+"_Ex", static_cast<float>(L1_met_Ex));
          auto L1_Ey = Monitored::Scalar<float>("L1_"+alg+"_Ey", static_cast<float>(L1_met_Ey));
          auto L1_Et = Monitored::Scalar<float>("L1_"+alg+"_Et", static_cast<float>(L1_met_Et));
          auto L1_sumEt = Monitored::Scalar<float>("L1_"+alg+"_sumEt", static_cast<float>(L1_met_sumEt));
          auto L1_Ex_log = Monitored::Scalar<float>("L1_"+alg+"_Ex_log", static_cast<float>(L1_met_Ex_log));
          auto L1_Ey_log = Monitored::Scalar<float>("L1_"+alg+"_Ey_log", static_cast<float>(L1_met_Ey_log));
          auto L1_Et_log = Monitored::Scalar<float>("L1_"+alg+"_Et_log", static_cast<float>(L1_met_Et_log));
          auto L1_sumEt_log = Monitored::Scalar<float>("L1_"+alg+"_sumEt_log", static_cast<float>(L1_met_sumEt_log));
          fill(tool, L1_Ex, L1_Ey, L1_Et, L1_sumEt,
               L1_Ex_log, L1_Ey_log, L1_Et_log, L1_sumEt_log);
        }
      }
    }

    // define TrigMissingET object
    const xAOD::TrigMissingET *hlt_met = 0;

    // status, component from HLT cell
    int nComponent = m_compNames.size();
    int nStatus = m_bitNames.size();
    if ( hlt_cell_met_cont.isValid() && hlt_cell_met_cont->size() > 0 ) {
      hlt_met = hlt_cell_met_cont->at(0);
      for (int j=0; j<nStatus; ++j) { //status loop
        unsigned mask = (1u<<j);
        if (hlt_met->flag() & mask) {
          MET_status = 1.;
        } else {
          MET_status = 0;
        }
        auto mon1 = Monitored::Scalar<std::string>( "HLT_MET_status",m_bitNames[j]);
        fill(tool,mon1,MET_status);
      }

      for (int i=0; i<nComponent; ++i) { //component loop
        float ex = hlt_met->exComponent(i)*0.001;
        float ey = hlt_met->eyComponent(i)*0.001;
        component_Et = sqrt(ex*ex+ey*ey);
        auto mon2 = Monitored::Scalar<std::string>( "HLT_MET_component",m_compNames[i]);
        fill(tool,mon2,component_Et);
      }

      for (int i=0; i<nComponent; ++i) { //component loop
        for (int j=0; j<nStatus; ++j) { //status loop
          unsigned mask = (1u<<j);
          if (hlt_met->statusComponent(i) & mask) {
            component_status_weight = 1.;
          } else {
            component_status_weight = 0;
          }
          auto mon_bit = Monitored::Scalar<std::string>( "component_status",m_bitNames[j]);
          auto mon_comp = Monitored::Scalar<std::string>( "component",m_compNames[i]);
          fill(tool,mon_comp,mon_bit,component_status_weight);
        }
      }
    }


    // access HLT MET values
    for (const std::string& alg : m_algsHLT) {
      if (alg == "cell" && hlt_cell_met_cont.isValid() && hlt_cell_met_cont->size() > 0) {
        hlt_met = hlt_cell_met_cont->at(0);
      } else if (alg == "mht" && hlt_mht_met_cont.isValid() && hlt_mht_met_cont->size() > 0) {
        hlt_met = hlt_mht_met_cont->at(0);
      } else if (alg == "tc" && hlt_tc_met_cont.isValid() && hlt_tc_met_cont->size() > 0) {
        hlt_met = hlt_tc_met_cont->at(0);
      } else if (alg == "tc_em" && hlt_tc_em_met_cont.isValid() && hlt_tc_em_met_cont->size() > 0) {
        hlt_met = hlt_tc_em_met_cont->at(0);
      } else if (alg == "tcpufit" && hlt_tcpufit_met_cont.isValid() && hlt_tcpufit_met_cont->size() > 0) {
        hlt_met = hlt_tcpufit_met_cont->at(0);
      } else if (alg == "trkmht" && hlt_trkmht_met_cont.isValid() && hlt_trkmht_met_cont->size() > 0) {
        hlt_met = hlt_trkmht_met_cont->at(0);
      } else if (alg == "pfsum" && hlt_pfsum_met_cont.isValid() && hlt_pfsum_met_cont->size() > 0) {
        hlt_met = hlt_pfsum_met_cont->at(0);
      } else if (alg == "pfsum_cssk" && hlt_pfsum_cssk_met_cont.isValid() && hlt_pfsum_cssk_met_cont->size() > 0) {
        hlt_met = hlt_pfsum_cssk_met_cont->at(0);
      } else if (alg == "pfsum_vssk" && hlt_pfsum_vssk_met_cont.isValid() && hlt_pfsum_vssk_met_cont->size() > 0) {
        hlt_met = hlt_pfsum_vssk_met_cont->at(0);
      } else if (alg == "pfopufit" && hlt_pfopufit_met_cont.isValid() && hlt_pfopufit_met_cont->size() > 0) {
        hlt_met = hlt_pfopufit_met_cont->at(0);
      } else if (alg == "cvfpufit" && hlt_cvfpufit_met_cont.isValid() && hlt_cvfpufit_met_cont->size() > 0) {
        hlt_met = hlt_cvfpufit_met_cont->at(0);
      } else if (alg == "mhtpufit_pf" && hlt_mhtpufit_pf_met_cont.isValid() && hlt_mhtpufit_pf_met_cont->size() > 0) {
        hlt_met = hlt_mhtpufit_pf_met_cont->at(0);
      } else if (alg == "mhtpufit_em" && hlt_mhtpufit_em_met_cont.isValid() && hlt_mhtpufit_em_met_cont->size() > 0) {
        hlt_met = hlt_mhtpufit_em_met_cont->at(0);
      } else {
        hlt_met = 0;
      }

      if ( hlt_met ) {
        float hlt_Ex = (hlt_met->ex())/1000.;
        float hlt_Ey = (hlt_met->ey())/1000.;
        float hlt_Ez = (hlt_met->ez())/1000.;
        float hlt_Et = std::sqrt(hlt_Ex*hlt_Ex + hlt_Ey*hlt_Ey);
        float hlt_sumEt = (hlt_met->sumEt())/1000.;
        float hlt_Ex_log = signed_log(hlt_Ex, epsilon);
        float hlt_Ey_log = signed_log(hlt_Ey, epsilon);
        float hlt_Et_log = signed_log(hlt_Et, epsilon);
        float hlt_sumEt_log = signed_log(hlt_sumEt, epsilon); 
        TVector3 v(hlt_Ex, hlt_Ey, hlt_Ez);
        float hlt_eta = v.Eta();
        float hlt_phi = v.Phi();

        auto met_Ex = Monitored::Scalar<float>(alg+"_Ex", static_cast<float>(hlt_Ex));
        auto met_Ey = Monitored::Scalar<float>(alg+"_Ey", static_cast<float>(hlt_Ey));
        auto met_Et = Monitored::Scalar<float>(alg+"_Et", static_cast<float>(hlt_Et));
        auto met_sumEt = Monitored::Scalar<float>(alg+"_sumEt", static_cast<float>(hlt_sumEt));
        auto met_Ex_log = Monitored::Scalar<float>(alg+"_Ex_log", static_cast<float>(hlt_Ex_log));
        auto met_Ey_log = Monitored::Scalar<float>(alg+"_Ey_log", static_cast<float>(hlt_Ey_log));
        auto met_Et_log = Monitored::Scalar<float>(alg+"_Et_log", static_cast<float>(hlt_Et_log));
        auto met_sumEt_log = Monitored::Scalar<float>(alg+"_sumEt_log", static_cast<float>(hlt_sumEt_log));
        auto met_eta = Monitored::Scalar<float>(alg+"_eta", static_cast<float>(hlt_eta));
        auto met_phi = Monitored::Scalar<float>(alg+"_phi", static_cast<float>(hlt_phi));
        fill(tool,met_Ex,met_Ey,met_Et,met_sumEt,
             met_Ex_log,met_Ey_log,met_Et_log,met_sumEt_log,
             met_eta,met_phi);
        ATH_MSG_DEBUG(alg << ": hlt_Et = " << hlt_Et);
      }
    }

    // access HLT  MET Expert values
    for (const std::string& alg : m_algsHLTExpert) {
      if (alg == "cell" && hlt_cell_met_cont.isValid() && hlt_cell_met_cont->size() > 0) {
        hlt_met = hlt_cell_met_cont->at(0);
      } else if (alg == "mht" && hlt_mht_met_cont.isValid() && hlt_mht_met_cont->size() > 0) {
        hlt_met = hlt_mht_met_cont->at(0);
      } else if (alg == "tc" && hlt_tc_met_cont.isValid() && hlt_tc_met_cont->size() > 0) {
        hlt_met = hlt_tc_met_cont->at(0);
      } else if (alg == "tc_em" && hlt_tc_em_met_cont.isValid() && hlt_tc_em_met_cont->size() > 0) {
        hlt_met = hlt_tc_em_met_cont->at(0);
      } else if (alg == "tcpufit" && hlt_tcpufit_met_cont.isValid() && hlt_tcpufit_met_cont->size() > 0) {
        hlt_met = hlt_tcpufit_met_cont->at(0);
      } else if (alg == "trkmht" && hlt_trkmht_met_cont.isValid() && hlt_trkmht_met_cont->size() > 0) {
        hlt_met = hlt_trkmht_met_cont->at(0);
      } else if (alg == "pfsum" && hlt_pfsum_met_cont.isValid() && hlt_pfsum_met_cont->size() > 0) {
        hlt_met = hlt_pfsum_met_cont->at(0);
      } else if (alg == "pfsum_cssk" && hlt_pfsum_cssk_met_cont.isValid() && hlt_pfsum_cssk_met_cont->size() > 0) {
        hlt_met = hlt_pfsum_cssk_met_cont->at(0);
      } else if (alg == "pfsum_vssk" && hlt_pfsum_vssk_met_cont.isValid() && hlt_pfsum_vssk_met_cont->size() > 0) {
        hlt_met = hlt_pfsum_vssk_met_cont->at(0);
      } else if (alg == "pfopufit" && hlt_pfopufit_met_cont.isValid() && hlt_pfopufit_met_cont->size() > 0) {
        hlt_met = hlt_pfopufit_met_cont->at(0);
      } else if (alg == "cvfpufit" && hlt_cvfpufit_met_cont.isValid() && hlt_cvfpufit_met_cont->size() > 0) {
        hlt_met = hlt_cvfpufit_met_cont->at(0);
      } else if (alg == "mhtpufit_pf" && hlt_mhtpufit_pf_met_cont.isValid() && hlt_mhtpufit_pf_met_cont->size() > 0) {
        hlt_met = hlt_mhtpufit_pf_met_cont->at(0);
      } else if (alg == "mhtpufit_em" && hlt_mhtpufit_em_met_cont.isValid() && hlt_mhtpufit_em_met_cont->size() > 0) {
        hlt_met = hlt_mhtpufit_em_met_cont->at(0);
      } else {
        hlt_met = 0;
      }

      if ( hlt_met ) {
        float hlt_Ex = (hlt_met->ex())/1000.;
        float hlt_Ey = (hlt_met->ey())/1000.;
        float hlt_Et = std::sqrt(hlt_Ex*hlt_Ex + hlt_Ey*hlt_Ey);

        auto met_Ex = Monitored::Scalar<float>(alg+"_Ex", static_cast<float>(hlt_Ex));
        auto met_Ey = Monitored::Scalar<float>(alg+"_Ey", static_cast<float>(hlt_Ey));
        auto met_Et = Monitored::Scalar<float>(alg+"_Et", static_cast<float>(hlt_Et));
        fill(tool,met_Ex,met_Ey,met_Et);
        ATH_MSG_DEBUG(alg << ": hlt_Et = " << hlt_Et);
      }
    }

    // efficiency plots
    for (const std::string& chain : m_l1Chains) {
      auto pass_chain = Monitored::Scalar<float>("pass_" + chain, static_cast<float>(getTrigDecisionTool()->isPassed(chain)));
      fill(tool, pass_chain,offline_Et_eff);
    }
    for (const std::string& chain : m_hltChains) {
      auto pass_chain = Monitored::Scalar<float>("pass_" + chain, static_cast<float>(getTrigDecisionTool()->isPassed(chain)));
      fill(tool, pass_chain,offline_Et_eff);
    }
    for (const std::string& chain : m_hltChainsVal) {
      auto pass_chain = Monitored::Scalar<float>("pass_" + chain, static_cast<float>(getTrigDecisionTool()->isPassed(chain)));
      fill(tool, pass_chain,offline_Et_eff);
    }
    for (const std::string& chain : m_hltChainsT0) {
      auto pass_chain = Monitored::Scalar<float>("pass_" + chain, static_cast<float>(getTrigDecisionTool()->isPassed(chain)));
      fill(tool, pass_chain,offline_Et_eff);
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
