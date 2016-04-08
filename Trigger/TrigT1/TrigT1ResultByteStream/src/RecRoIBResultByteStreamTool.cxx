/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "TrigT1ResultByteStream/RecRoIBResultByteStreamTool.h"

// Include for the Level-1 configuration
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"

/// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IRecRoIBResultByteStreamTool( "RecRoIBResultByteStreamTool", 1, 1 );

/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID& RecRoIBResultByteStreamTool::interfaceID( ) {
  return IID_IRecRoIBResultByteStreamTool;
}

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
RecRoIBResultByteStreamTool::RecRoIBResultByteStreamTool( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : AthAlgTool( type, name, parent ),
    m_detectorStore( "DetectorStore", name ),
    m_rpcRoISvc( LVL1::ID_RecRpcRoiSvc, name ),
    m_tgcRoISvc( LVL1::ID_RecTgcRoiSvc, name ),
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
    m_muRoISvcPresent( true ) {

  declareInterface< RecRoIBResultByteStreamTool >( this );

}

/**
 * The destructor doesn't do anything.
 */
RecRoIBResultByteStreamTool::~RecRoIBResultByteStreamTool() {

}

/**
 * The function connects to all services being used,
 * and initialises the base class.
 */
StatusCode RecRoIBResultByteStreamTool::initialize() {

  StatusCode sc = m_detectorStore.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_FATAL("Couldn't access DetectorStore");
    return sc;
  } else {
    ATH_MSG_DEBUG("Connected to DetectorStore");
  }
  
  /// Connect to the LVL1ConfigSvc for the trigger configuration:
  sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Couldn't connect to Lvl1ConfigSvc.");
  } else {
    /// Load the threshold vectors
    std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
    for (std::vector<TrigConf::TriggerThreshold*>::const_iterator it = thresholds.begin();
         it != thresholds.end(); ++it) {
       if ( (*it)->type() == TrigConf::L1DataDef::muonType() )     m_muonConfig.push_back(*it);
       else if ( (*it)->type() == TrigConf::L1DataDef::emType() ||
                 (*it)->type() == TrigConf::L1DataDef::tauType() ) m_emtauConfig.push_back(*it);
       else if ( (*it)->type() == TrigConf::L1DataDef::jetType() ||
                 (*it)->type() == TrigConf::L1DataDef::jfType()  ||
                 (*it)->type() == TrigConf::L1DataDef::jbType() )  m_jetConfig.push_back(*it);
    }    
  }
    
  m_muRoISvcPresent = true;

  sc = m_rpcRoISvc.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_WARNING("Couldn't access RPC RecMuonRoISvc");
    ATH_MSG_WARNING("Muon coordinates will be zero!");
    m_muRoISvcPresent = false;
  } else {
    ATH_MSG_DEBUG("Connected to RPC RecMuonRoISvc");
  }

  sc = m_tgcRoISvc.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_WARNING("Couldn't access TGC RecMuonRoISvc");
    ATH_MSG_WARNING("Muon coordinates will be zero!");
    m_muRoISvcPresent = false;
  } else {
    ATH_MSG_DEBUG("Connected to TGC RecMuonRoISvc");
  }

  return StatusCode::SUCCESS;

}

/**
 * The function finalises the base class.
 */
StatusCode RecRoIBResultByteStreamTool::finalize() {
  return StatusCode::SUCCESS;
}
