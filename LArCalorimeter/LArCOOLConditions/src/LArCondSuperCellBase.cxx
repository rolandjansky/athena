/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArCondSuperCellBase.h"


// Services/helpers
//#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

// Gaudi/Athena
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/getMessageSvc.h"

LArCondSuperCellBase::LArCondSuperCellBase() :
  m_isInitialized(false),
  m_scOnlineID(NULL),
  m_scCablingTool(NULL),
  m_log(NULL) {}

LArCondSuperCellBase::~LArCondSuperCellBase() {
  delete m_log;
}
  
StatusCode LArCondSuperCellBase::initializeBase(const char* context) {

  std::string sContext;
  if (context==NULL || context[0]=='\0')//empty string
    sContext="LArCondSuperCellBase";
  else
    sContext=context;

  if (!m_log)
    m_log=new MsgStream(Athena::getMessageSvc(), sContext);
  (*m_log) << MSG::DEBUG << "initializeBase "<< endreq;

  if (m_isInitialized) {
    (*m_log) << MSG::DEBUG << "already initialized - returning "<< endreq;
    return (StatusCode::SUCCESS);
  }
  //Get SuperCellID ...
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StoreGateSvc* detStore;
  StatusCode sc = svcLoc->service("DetectorStore",detStore);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }
  sc = detStore->retrieve( m_scOnlineID,"LArOnline_SuperCellID");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Cannot get LArOnline_SuperCellID!" << endreq;
    return sc;
  }

  m_isInitialized = true;
  (*m_log) << MSG::DEBUG << "end initializeBase " << endreq;
  return (StatusCode::SUCCESS);
}
