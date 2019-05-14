/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResult.h"
#include "xAODTrigger/TrigNavigation.h"
#include "xAODTrigger/TrigNavigationAuxInfo.h"
#include "TrigNavigationThinningTool.h"
#include "SlimmingHandler.h"

TrigNavigationThinningTool::TrigNavigationThinningTool(const std::string& type, const std::string& name, const IInterface* parent) 
    : AthAlgTool(type, name, parent), 
      m_thinningSvc("IThinningSvc/ThinningSvc", name),
      m_slimmingTool("", this)  
      //      m_slimmingTool("HLT::TrigNavigationSlimmingTool/TrigNavigationSlimmingTool", this)  // a private instansce
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningSvc", m_thinningSvc, "Synchronize feature indexes with this instance of ThinningSvc");
  declareProperty("SlimmingTool", m_slimmingTool, "Tool instance which does the actual slimming");
  declareProperty("ResultKey", m_HLTResultKey = "HLTResult_HLT");
  declareProperty("xAODNavigationKey", m_xAODNavigationKey = "TrigNavigation");
  declareProperty("ActInPlace", m_actInPlace = false, "Run slimming on the spot (creates messes when multiple output streams are produced by the job!)");
}

TrigNavigationThinningTool::~TrigNavigationThinningTool(){
}

StatusCode TrigNavigationThinningTool::initialize() {
  ATH_MSG_DEBUG( "TrigNavigationThinningTool::initialize()" );
  CHECK( m_thinningSvc.retrieve() ); 
  CHECK( m_slimmingTool.retrieve() );
  CHECK( m_slimmingTool->initialize() );
  return  StatusCode::SUCCESS;
}
StatusCode TrigNavigationThinningTool::finalize() {
  return  StatusCode::SUCCESS;
}


template<>
xAOD::TrigNavigation* TrigNavigationThinningTool::createAndRecordResult(const std::string& key) const { 
  auto nav = new xAOD::TrigNavigation();
  auto store = new xAOD::TrigNavigationAuxInfo();
  nav->setStore(store);

  if ( evtStore()->record(nav, key).isFailure() ) {
    ATH_MSG_ERROR( "Can not record the result xAODTrigNaviagtion in store, this is badly functioning and configuration needs fixing");      
    return nullptr;
  }

  if ( evtStore()->record(store, key+"Aux.").isFailure() ) {
    ATH_MSG_ERROR( "Can not record the result (Aux) in store, this is badly functioning and configuration needs fixing");      
    return nullptr;
  }

  return nav;  
}


StatusCode TrigNavigationThinningTool::doThinning() const {
  ATH_MSG_DEBUG( "TrigNavigationThinningTool::doThinning()" );

  // Check if there is a (const) HLTResult
  HLT::HLTResult *hltresult = nullptr;
  if ( not m_HLTResultKey.empty() and evtStore()->contains<HLT::HLTResult>(m_HLTResultKey) ) {
    const HLT::HLTResult* cres = nullptr;
    if ( evtStore()->retrieve( cres, m_HLTResultKey).isFailure() or cres==nullptr ) {
      ATH_MSG_WARNING( "Unable to load HLT::HLTResult using key " << m_HLTResultKey );
    }
    else {
      hltresult = const_cast<HLT::HLTResult*>(cres);
    }
  }

  // Check if there is a xAOD::TrigNavigation, or create it
  xAOD::TrigNavigation *xaodresult = nullptr;
  if ( not m_xAODNavigationKey.empty() ) {
    if ( not evtStore()->contains<xAOD::TrigNavigation>(m_xAODNavigationKey) ) {
      ATH_MSG_DEBUG( "Absent xAOD::TrigNavigation of key " << m_xAODNavigationKey << ". Creating new one and registering it");
      xaodresult = createAndRecordResult<xAOD::TrigNavigation>(m_xAODNavigationKey);
    }
    if ( evtStore()->retrieve( xaodresult, m_xAODNavigationKey).isFailure() or xaodresult==nullptr ) {
      ATH_MSG_WARNING( "Unable to load xAOD::TrigNavigation using key " << m_xAODNavigationKey );
    }
  }

  if ( hltresult == nullptr and xaodresult == nullptr ) {
    ATH_MSG_DEBUG( "Neither xAOD, not HLT navigations are present or required, doing nothing");
    return StatusCode::SUCCESS;
  }


  if ( m_actInPlace ) {
    ATH_MSG_DEBUG( "Running navigation slimming in place" );
    std::vector<uint32_t> slimmedPayload;
    CHECK( m_slimmingTool->doSlimming( slimmedPayload) );
    if ( hltresult ) {
      hltresult->getNavigationResultCuts().clear();
      hltresult->getNavigationResult() = slimmedPayload;      
    }
    if ( xaodresult ) {
      xaodresult->setSerialized(slimmedPayload);
    }
  } else {
    ATH_MSG_DEBUG( "Scheduling trigger navigation slimming while writing output file" );
    if ( hltresult ) {
      SlimmingHandler<HLT::HLTResult> *hdlr 
	= new SlimmingHandler<HLT::HLTResult>(hltresult, this, m_slimmingTool.operator->());      
      CHECK(m_thinningSvc->register_slimmer(hdlr));
    }
    if (xaodresult ) {
      SlimmingHandler<xAOD::TrigNavigation> *hdlr 
	= new SlimmingHandler<xAOD::TrigNavigation>(xaodresult, this, m_slimmingTool.operator->());      
      CHECK(m_thinningSvc->register_slimmer(hdlr));
      
    }


  }
  return StatusCode::SUCCESS;
}


