/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitorAlgorithm.h"
#include <TVector3.h>

TrigMETMonitorAlgorithm::TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  , m_offline_met_key("MET_EMTopo")
  , m_lvl1_roi_key("LVL1EnergySumRoI")
  , m_hlt_cell_met_key("HLT_MET_cell")
  , m_hlt_mht_met_key("HLT_MET_mht")
  , m_hlt_tc_met_key("HLT_MET_tc")
  , m_hlt_tcpufit_met_key("HLT_MET_tcpufit")
  , m_hlt_trkmht_met_key("HLT_MET_trkmht")
  , m_hlt_pfsum_met_key("HLT_MET_pfsum")
  , m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty("offline_met_key", m_offline_met_key);
  declareProperty("l1_roi_key", m_lvl1_roi_key);
  declareProperty("hlt_cell_key", m_hlt_cell_met_key);
  declareProperty("hlt_mht_key", m_hlt_mht_met_key);
  declareProperty("hlt_tc_key", m_hlt_tc_met_key);
  declareProperty("hlt_tcpufit_key", m_hlt_tcpufit_met_key);
  declareProperty("hlt_trkmht_key", m_hlt_trkmht_met_key);
  declareProperty("hlt_pfsum_key", m_hlt_pfsum_met_key);

  declareProperty("L1Chain1", m_L1Chain1="L1_XE50");
  declareProperty("HLTChain1", m_HLTChain1="HLT_xe65_cell_L1XE50");
  declareProperty("HLTChain2", m_HLTChain2="HLT_xe100_tcpuft_L1XE50");
}


TrigMETMonitorAlgorithm::~TrigMETMonitorAlgorithm() {}


StatusCode TrigMETMonitorAlgorithm::initialize() {
    ATH_CHECK( m_offline_met_key.initialize() );
    ATH_CHECK( m_lvl1_roi_key.initialize() );
    ATH_CHECK( m_hlt_cell_met_key.initialize() );
    ATH_CHECK( m_hlt_mht_met_key.initialize() );
    ATH_CHECK( m_hlt_tc_met_key.initialize() );
    ATH_CHECK( m_hlt_tcpufit_met_key.initialize() );
    ATH_CHECK( m_hlt_trkmht_met_key.initialize() );
    ATH_CHECK( m_hlt_pfsum_met_key.initialize() );

    ATH_CHECK( m_trigDecTool.retrieve() );

    return AthMonitorAlgorithm::initialize();
}


StatusCode TrigMETMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // access met containers
    SG::ReadHandle<xAOD::MissingETContainer> offline_met_cont(m_offline_met_key, ctx);
    if (! offline_met_cont.isValid() || offline_met_cont->size()==0 ) {
      ATH_MSG_DEBUG("Container "<< m_offline_met_key << " does not exist or is empty");
    }

    SG::ReadHandle<xAOD::EnergySumRoI> l1_roi_cont(m_lvl1_roi_key, ctx);
    if (! l1_roi_cont.isValid() ) {     
      ATH_MSG_DEBUG("Container "<< m_lvl1_roi_key << " does not exist or is empty");
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
   

    // define offline MissingET object
    const xAOD::MissingET *offline_met = 0;

    // define TrigMissingET object
    const xAOD::TrigMissingET *hlt_met = 0;

    // define variables
    auto offline_Ex = Monitored::Scalar<float>("offline_Ex",0.0);
    auto offline_Ey = Monitored::Scalar<float>("offline_Ey",0.0);
    auto offline_Et = Monitored::Scalar<float>("offline_Et",0.0);
    auto offline_sumEt = Monitored::Scalar<float>("offline_sumEt",0.0);
    auto L1_Ex = Monitored::Scalar<float>("L1_Ex",0.0);
    auto L1_Ey = Monitored::Scalar<float>("L1_Ey",0.0);
    auto L1_Et = Monitored::Scalar<float>("L1_Et",0.0);
    auto cell_Ex = Monitored::Scalar<float>("cell_Ex",0.0);
    auto cell_Ey = Monitored::Scalar<float>("cell_Ey",0.0);
    auto cell_Et = Monitored::Scalar<float>("cell_Et",0.0);
    auto mht_Ex = Monitored::Scalar<float>("mht_Ex",0.0);
    auto mht_Ey = Monitored::Scalar<float>("mht_Ey",0.0);
    auto mht_Et = Monitored::Scalar<float>("mht_Et",0.0);
    auto tc_Ex = Monitored::Scalar<float>("tc_Ex",0.0);
    auto tc_Ey = Monitored::Scalar<float>("tc_Ey",0.0);
    auto tc_Et = Monitored::Scalar<float>("tc_Et",0.0);
    auto trkmht_Ex = Monitored::Scalar<float>("trkmht_Ex",0.0);
    auto trkmht_Ey = Monitored::Scalar<float>("trkmht_Ey",0.0);
    auto trkmht_Et = Monitored::Scalar<float>("trkmht_Et",0.0);
    auto tcpufit_Ex = Monitored::Scalar<float>("tcpufit_Ex",0.0);
    auto tcpufit_Ey = Monitored::Scalar<float>("tcpufit_Ey",0.0);
    auto tcpufit_Ez = Monitored::Scalar<float>("tcpufit_Ez",0.0);
    auto tcpufit_Et = Monitored::Scalar<float>("tcpufit_Et",0.0);
    auto tcpufit_sumEt = Monitored::Scalar<float>("tcpufit_sumEt",0.0);
    auto tcpufit_sumE = Monitored::Scalar<float>("tcpufit_sumE",0.0);
    auto tcpufit_eta = Monitored::Scalar<float>("tcpufit_eta",0.0);
    auto tcpufit_phi = Monitored::Scalar<float>("tcpufit_phi",0.0);
    auto pfsum_Ex = Monitored::Scalar<float>("pfsum_Ex",0.0);
    auto pfsum_Ey = Monitored::Scalar<float>("pfsum_Ey",0.0);
    auto pfsum_Et = Monitored::Scalar<float>("pfsum_Et",0.0);
    auto pass_L11 = Monitored::Scalar<float>("pass_L11",0.0);
    auto pass_HLT1 = Monitored::Scalar<float>("pass_HLT1",0.0);
    auto pass_HLT2 = Monitored::Scalar<float>("pass_HLT2",0.0);

    // access offline MET values
    if ( offline_met_cont.isValid() && offline_met_cont->size() > 0 ) {
      offline_met = offline_met_cont->at(0);
      offline_Ex = - (offline_met->mpx())/1000.;
      offline_Ey = - (offline_met->mpy())/1000.;
      offline_sumEt = (offline_met->sumet())/1000.;
      offline_Et = sqrt(offline_Ex*offline_Ex + offline_Ey*offline_Ey);
    }

    // access L1 MET values
    if ( l1_roi_cont.isValid() ) {
      if ((l1_roi_cont->energyX())>-9e12 && (l1_roi_cont->energyX())<9e12 && (l1_roi_cont->energyY())>-9e12 && (l1_roi_cont->energyY())<9e12) { 
	L1_Ex = - (l1_roi_cont->energyX())/1000.;
	L1_Ey = - (l1_roi_cont->energyY())/1000.;
	L1_Et = (l1_roi_cont->energyT())/1000.;
      }
    }

    // access HLT cell MET values
    if ( hlt_cell_met_cont.isValid() && hlt_cell_met_cont->size() > 0 ) {
      hlt_met = hlt_cell_met_cont->at(0);
      cell_Ex = (hlt_met->ex())/1000.;
      cell_Ey = (hlt_met->ey())/1000.;
      cell_Et = sqrt(cell_Ex*cell_Ex + cell_Ey*cell_Ey);
    }

    // access HLT mht MET values
    if ( hlt_mht_met_cont.isValid() && hlt_mht_met_cont->size() > 0 ) {
      hlt_met = hlt_mht_met_cont->at(0);
      mht_Ex = (hlt_met->ex())/1000.;
      mht_Ey = (hlt_met->ey())/1000.;
      mht_Et = sqrt(mht_Ex*mht_Ex + mht_Ey*mht_Ey);
    }

    // access HLT tclcw MET values
    if ( hlt_tc_met_cont.isValid() && hlt_tc_met_cont->size() > 0 ) {
      hlt_met = hlt_tc_met_cont->at(0);
      tc_Ex = (hlt_met->ex())/1000.;
      tc_Ey = (hlt_met->ey())/1000.;
      tc_Et = sqrt(tc_Ex*tc_Ex + tc_Ey*tc_Ey);
    }

    // access HLT trkmht MET values
    if ( hlt_trkmht_met_cont.isValid() && hlt_trkmht_met_cont->size() > 0 ) {
      hlt_met = hlt_trkmht_met_cont->at(0);
      trkmht_Ex = (hlt_met->ex())/1000.;
      trkmht_Ey = (hlt_met->ey())/1000.;
      trkmht_Et = sqrt(trkmht_Ex*trkmht_Ex + trkmht_Ey*trkmht_Ey);
    }

    // access HLT tcpufit MET values
    if ( hlt_tcpufit_met_cont.isValid() && hlt_tcpufit_met_cont->size() > 0 ) {
      hlt_met = hlt_tcpufit_met_cont->at(0);
      tcpufit_Ex = (hlt_met->ex())/1000.;
      tcpufit_Ey = (hlt_met->ey())/1000.;
      tcpufit_Ez = (hlt_met->ez())/1000.;
      tcpufit_Et = sqrt(tcpufit_Ex*tcpufit_Ex + tcpufit_Ey*tcpufit_Ey);
      tcpufit_sumEt = (hlt_met->sumEt())/1000.;
      tcpufit_sumE = (hlt_met->sumE())/1000.;

      TVector3 v(tcpufit_Ex, tcpufit_Ey, tcpufit_Ez);
      tcpufit_eta = v.Eta();
      tcpufit_phi = v.Phi();
    }

    // access HLT pfsum MET values
    if ( hlt_pfsum_met_cont.isValid() && hlt_pfsum_met_cont->size() > 0 ) {
      hlt_met = hlt_pfsum_met_cont->at(0);
      pfsum_Ex = (hlt_met->ex())/1000.;
      pfsum_Ey = (hlt_met->ey())/1000.;
      pfsum_Et = sqrt(pfsum_Ex*pfsum_Ex + pfsum_Ey*pfsum_Ey);
    }

    // efficiency plots
    if (m_trigDecTool->isPassed(m_L1Chain1)) pass_L11 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain1)) pass_HLT1 = 1.0;
    if (m_trigDecTool->isPassed(m_HLTChain2)) pass_HLT2 = 1.0;
    ATH_MSG_DEBUG("pass " << m_L1Chain1 << " = " << pass_L11);
    ATH_MSG_DEBUG("pass " << m_HLTChain1 << " = " << pass_HLT1);
    ATH_MSG_DEBUG("pass " << m_HLTChain2 << " = " << pass_HLT2);


    // check active triggers
    // This does not work for now
    /*
    auto chaingroup = m_trigDecTool->getChainGroup("HLT_xe.*");
    for(auto &trig : chainGroup->getListOfTriggers()) {
      auto cg = m_trigDecTool->getChainGroup(trig);
      std::string thisTrig = trig;
      ATH_MSG_DEBUG (thisTrig << " chain prescale = " << cg->getPrescale());
    }
     */


    // Fill. First argument is the tool (GMT) name as defined in the py file, 
    // all others are the variables to be saved.
    //fill("TrigMETMonitor",L1_Ex,L1_Ey,L1_Et,pass_HLT1);
    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigMETMonitor");
    fill(tool,offline_Ex,offline_Ey,offline_Et,offline_sumEt,
         L1_Ex,L1_Ey,L1_Et,
         pass_L11,pass_HLT1,pass_HLT2);
    if (hlt_cell_met_cont->size() > 0) {
      fill(tool,cell_Ex,cell_Ey,cell_Et,
         mht_Ex,mht_Ey,mht_Et,
         tc_Ex,tc_Ey,tc_Et,
         trkmht_Ex,trkmht_Ey,trkmht_Et,
         pfsum_Ex,pfsum_Ey,pfsum_Et,
         tcpufit_Ex,tcpufit_Ey,tcpufit_Ez,tcpufit_Et,
         tcpufit_sumEt,tcpufit_sumE,tcpufit_eta,tcpufit_phi);
    }

    return StatusCode::SUCCESS;
}
