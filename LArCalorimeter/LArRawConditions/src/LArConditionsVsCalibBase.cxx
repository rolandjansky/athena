//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArConditionsVsCalibBase.h"


StatusCode LArConditionsVsCalibBase::initialize() {
  MsgStream log(Athena::getMessageSvc(), "LArConditionsVsCalibBase");

  if ( m_init )  return StatusCode::SUCCESS ;

  ISvcLocator* svcLoc = Gaudi::svcLocator() ;
  StatusCode sc = svcLoc->service("DetectorStore",m_detStore) ;
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Could not get detector store" << endmsg ;
    return sc ;
  }
  sc = m_detStore->retrieve(m_onlineHelper,"LArOnlineID") ;
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Could not get online helper" << endmsg ;
    return sc ;
  }

  IToolSvc* toolSvc ;
  sc = svcLoc->service("ToolSvc",toolSvc) ;
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Could not get ToolSvc" << endmsg ;
    return sc ;
  }
  sc = toolSvc->retrieveTool("LArCablingService",m_cablingSvc) ;
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Could not get cabling service" << endmsg ; 
    return sc ;
  }

  m_init = true ;
  log << MSG::INFO << "successfully initialized ..." << endmsg ;
  return StatusCode::SUCCESS ;
}

HWIdentifier LArConditionsVsCalibBase::getCalibLine(HWIdentifier ChID) const {
  MsgStream log(Athena::getMessageSvc(), "LArConditionsVsCalibBase");
  log << MSG::VERBOSE << "getCalibLine(HWIdentifier): just entered" << endmsg ;
  if ( ! m_init ) {
    log << MSG::ERROR << "getCalibLine(HWIdentifier): not initialized!" << endmsg ;
    return HWIdentifier(0) ;
  }
  //-------------------------------------------------------
  static bool first_time = true ;
  if ( first_time ) {
    // this forces loading the OnOffIdMap, which also contains readout->calib mapping
    // if not done, all the rest fails!
    log << MSG::INFO << "getCalibLine(HWIdentifier): force loading of the OnOffIdMap" << endmsg ;
    try {
      log << MSG::VERBOSE << "getCalibLine(HWIdentifier): dummy call to m_cablingSvc->cnvToIdentifier(ChID), ChID="
	  << ChID.getString() << endmsg ;
      Identifier id = m_cablingSvc->cnvToIdentifier(ChID);
      // this VERBOSE printout avoid compilation warning "unused variable `Identifier id'"
      log << MSG::VERBOSE << "getCalibLine(HWIdentifier): dummy call returned Identifier ID = 0x" << MSG::hex << id << MSG::dec << endmsg ;
    } catch ( LArID_Exception & except ) {
      log << MSG::VERBOSE << "getCalibLine(HWIdentifier): m_cablingSvc->cnvToIdentifier(ChID) failed" << endmsg ;
      if ( m_onlineHelper->isCalibration(ChID) ) {
          log << MSG::VERBOSE << ChID.getString() << " is calibration line" << endmsg ;
      } else {
          log << MSG::VERBOSE << ChID.getString() << " isn't calibration line" << endmsg ;
      }
    }
    log << MSG::INFO << "getCalibLine(HWIdentifier): OnOffIdMap loaded (hopefully!)" << endmsg ;
    first_time = false ;
  }
  //-------------------------------------------------------
  if ( m_onlineHelper->isCalibration(ChID) ) {
      log << MSG::VERBOSE << "getCalibLine(HWIdentifier): " << ChID.getString() << " is already a calib line" << endmsg ;
    return ChID ;
  }
  const std::vector<HWIdentifier>& calibLineV = m_cablingSvc->calibSlotLine(ChID) ;
  if ( calibLineV.size() > 0 ) {
    return calibLineV[0] ; 
  } else {
      log << MSG::WARNING << "Could not get calib line for channel " << ChID.getString() << endmsg ;
    return HWIdentifier(0) ;
  }
}

// inline HWIdentifier LArConditionsVsCalibBase::getSlotID(HWIdentifier ChID) const {
//   //MsgStream log(Athena::getMessageSvc(), "LArConditionsVsCalibBase");
//   //int b_ec, pn, ft, slot ;
//   return m_onlineHelper->calib_module_Id( ChID ) ;
// }
