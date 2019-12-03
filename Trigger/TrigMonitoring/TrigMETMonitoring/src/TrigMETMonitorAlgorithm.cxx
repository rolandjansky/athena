/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitorAlgorithm.h"

TrigMETMonitorAlgorithm::TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  , m_lvl1_roi_key("LVL1EnergySumRoI")
  , m_hlt_cell_met_key("HLT_MET_cell")
  , m_hlt_mht_met_key("HLT_MET_mht")
  , m_hlt_tc_met_key("HLT_MET_tc")
  , m_hlt_tcpufit_met_key("HLT_MET_tcPufit")
{
  declareProperty("l1_roi_key", m_lvl1_roi_key);
  declareProperty("hlt_cell_key", m_hlt_cell_met_key);
  declareProperty("hlt_mht_key", m_hlt_mht_met_key);
  declareProperty("hlt_tc_key", m_hlt_tc_met_key);
  declareProperty("hlt_tcpufit_key", m_hlt_tcpufit_met_key);
}


TrigMETMonitorAlgorithm::~TrigMETMonitorAlgorithm() {}


StatusCode TrigMETMonitorAlgorithm::initialize() {
    ATH_CHECK( m_lvl1_roi_key.initialize() );
    ATH_CHECK( m_hlt_cell_met_key.initialize() );
    ATH_CHECK( m_hlt_mht_met_key.initialize() );
    ATH_CHECK( m_hlt_tc_met_key.initialize() );
    ATH_CHECK( m_hlt_tcpufit_met_key.initialize() );

    return AthMonitorAlgorithm::initialize();
}


StatusCode TrigMETMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // access met containers
    SG::ReadHandle<xAOD::EnergySumRoI> l1_roi_cont(m_lvl1_roi_key, ctx);
    if (! l1_roi_cont.isValid() ) {     
      ATH_MSG_WARNING("evtStore() does not contain L1 MET Collection with name "<< m_lvl1_roi_key);
      //return StatusCode::FAILURE;
    }
    
    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_cell_met_cont(m_hlt_cell_met_key, ctx);
    if (hlt_cell_met_cont->size()==0 || ! hlt_cell_met_cont.isValid() ) {
      ATH_MSG_WARNING("evtStore() does not contain HLT MET Collection with name "<< m_hlt_cell_met_key);
      //return StatusCode::FAILURE;
    }
    
    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_mht_met_cont(m_hlt_mht_met_key, ctx);
    if (hlt_mht_met_cont->size()==0 || ! hlt_mht_met_cont.isValid() ) {
	ATH_MSG_WARNING("evtStore() does not contain HLT MET Collection with name "<< m_hlt_mht_met_key);
	//return StatusCode::FAILURE;
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_tc_met_cont(m_hlt_tc_met_key, ctx);
    if (hlt_tc_met_cont->size()==0 || ! hlt_tc_met_cont.isValid() ) {
	ATH_MSG_WARNING("evtStore() does not contain HLT MET Collection with name "<< m_hlt_tc_met_key);
	//return StatusCode::FAILURE;
    }

    SG::ReadHandle<xAOD::TrigMissingETContainer> hlt_tcpufit_met_cont(m_hlt_tcpufit_met_key, ctx);
    if (hlt_tcpufit_met_cont->size()==0 || ! hlt_tcpufit_met_cont.isValid() ) {
	ATH_MSG_WARNING("evtStore() does not contain HLT MET Collection with name "<< m_hlt_tcpufit_met_key);
	//return StatusCode::FAILURE;
    }
    
    // define TrigMissingET object
    const xAOD::TrigMissingET *hlt_met = 0;

    // define variables
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
    auto tcpufit_Ex = Monitored::Scalar<float>("tcpufit_Ex",0.0);
    auto tcpufit_Ey = Monitored::Scalar<float>("tcpufit_Ey",0.0);
    auto tcpufit_Et = Monitored::Scalar<float>("tcpufit_Et",0.0);

    // access L1 MET values
    // The following code was commented till we can get a proper input AOD file
    /*
    if ( l1_roi_cont.isValid() ) {
      if ((l1_roi_cont->energyX())>-9e12 && (l1_roi_cont->energyX())<9e12 && (l1_roi_cont->energyY())>-9e12 && (l1_roi_cont->energyY())<9e12) { 
	L1_Ex = - (l1_roi_cont->energyX())/1000.;
	L1_Ey = - (l1_roi_cont->energyY())/1000.;
	L1_Et = sqrt(L1_Ex*L1_Ex + L1_Ey*L1_Ey);
      }
    }
    */

    // access HLT cell MET values
    if ( hlt_cell_met_cont->size() > 0 && hlt_cell_met_cont.isValid() ) {
      hlt_met = hlt_cell_met_cont->at(0);
      cell_Ex = (hlt_met->ex())/1000.;
      cell_Ey = (hlt_met->ey())/1000.;
      cell_Et = sqrt(cell_Ex*cell_Ex + cell_Ey*cell_Ey);
    }

    // access HLT mht MET values
    if ( hlt_mht_met_cont->size() > 0 && hlt_mht_met_cont.isValid() ) {
      hlt_met = hlt_mht_met_cont->at(0);
      mht_Ex = (hlt_met->ex())/1000.;
      mht_Ey = (hlt_met->ey())/1000.;
      mht_Et = sqrt(mht_Ex*mht_Ex + mht_Ey*mht_Ey);
    }

    // access HLT tclcw MET values
    if ( hlt_tc_met_cont->size() > 0 && hlt_tc_met_cont.isValid() ) {
      hlt_met = hlt_tc_met_cont->at(0);
      tc_Ex = (hlt_met->ex())/1000.;
      tc_Ey = (hlt_met->ey())/1000.;
      tc_Et = sqrt(tc_Ex*tc_Ex + tc_Ey*tc_Ey);
    }

    // access HLT tcPufit MET values
    if ( hlt_tcpufit_met_cont->size() > 0 && hlt_tcpufit_met_cont.isValid() ) {
      hlt_met = hlt_tcpufit_met_cont->at(0);
      tcpufit_Ex = (hlt_met->ex())/1000.;
      tcpufit_Ey = (hlt_met->ey())/1000.;
      tcpufit_Et = sqrt(tcpufit_Ex*tcpufit_Ex + tcpufit_Ey*tcpufit_Ey);
    }

    const auto& trigDecTool = getTrigDecisionTool();
    // TDT test
    if (trigDecTool->isPassed("HLT_xe30_cell_L1XE10")) {
      ATH_MSG_DEBUG("passed HLT_xe30_cell_L1XE10");
    } else {
      ATH_MSG_DEBUG("not passed HLT_xe30_cell_L1XE10");
    }
    if (trigDecTool->isPassed("HLT_xe30_cell_xe30_tcpufit_L1XE10")) {
      ATH_MSG_DEBUG("passed HLT_xe30_cell_xe30_tcpufit_L1XE10");
    } else {
      ATH_MSG_DEBUG("not passed HLT_xe30_cell_xe30_tcpufit_L1XE10");
    }
    if (trigDecTool->isPassed("HLT_xe30_tcpufit_L1XE10")) {
      ATH_MSG_DEBUG("passed HLT_xe30_tcpufit_L1XE10");
    } else {
      ATH_MSG_DEBUG("not passed HLT_xe30_tcpufit_L1XE10");
    }
    if (trigDecTool->isPassed(m_triggerChainString)) {
      ATH_MSG_DEBUG("passed " << m_triggerChainString);
    } else {
      ATH_MSG_DEBUG("not passed " << m_triggerChainString);
    }

    // check active triggers
    // This does not work for now
    /*
    auto chaingroup = trigDecTool->getChainGroup("HLT_xe.*");
    for(auto &trig : chainGroup->getListOfTriggers()) {
      auto cg = trigDecTool->getChainGroup(trig);
      std::string thisTrig = trig;
      ATH_MSG_DEBUG (thisTrig << " chain prescale = " << cg->getPrescale());
    }
     */


    // Fill. First argument is the tool (GMT) name as defined in the py file, 
    // all others are the variables to be saved.
    fill("TrigMETMonitor",L1_Ex,L1_Ey,L1_Et);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigMETMonitor");
    fill(tool,cell_Ex,cell_Ey,cell_Et);
    fill(tool,mht_Ex,mht_Ey,mht_Et);
    fill(tool,tc_Ex,tc_Ey,tc_Et);
    fill(tool,tcpufit_Ex,tcpufit_Ey,tcpufit_Et);

    return StatusCode::SUCCESS;
}
