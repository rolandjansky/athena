/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArCondFlatBase.h"


// Services/helpers
#include "LArIdentifier/LArOnlineID.h"

// Gaudi/Athena
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/getMessageSvc.h"

LArCondFlatBase::LArCondFlatBase() :
  m_isInitialized(false),
  m_onlineHelper(NULL),
  m_log(NULL) {}

LArCondFlatBase::~LArCondFlatBase() {
  delete m_log;
}
  
StatusCode LArCondFlatBase::initializeBase(const char* context) {

  std::string sContext;
  if (context==NULL || context[0]=='\0')//empty string
    sContext="LArCondFlatBase";
  else
    sContext=context;
  m_log=new MsgStream(Athena::getMessageSvc(), sContext);
  (*m_log) << MSG::DEBUG << "initializeBase "<< endmsg;

  if (m_isInitialized) {
    (*m_log) << MSG::DEBUG << "already initialized - returning "<< endmsg;
    return (StatusCode::SUCCESS);
  }
  //Get LArOnlineID....
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StoreGateSvc* detStore;
  StatusCode sc = svcLoc->service("DetectorStore",detStore);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Cannot get DetectorStore!" << endmsg;
    return sc;
  }
  sc = detStore->retrieve(m_onlineHelper,"LArOnlineID");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Cannot get LArOnlineID!" << endmsg;
    return sc;
  }

  IToolSvc* toolSvc;
  sc = svcLoc->service( "ToolSvc",toolSvc  );
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Cannot get ToolSvc!" << endmsg;
    return sc;
  }

  m_isInitialized = true;
  (*m_log) << MSG::DEBUG << "end initializeBase " << endmsg;
  return (StatusCode::SUCCESS);
}
