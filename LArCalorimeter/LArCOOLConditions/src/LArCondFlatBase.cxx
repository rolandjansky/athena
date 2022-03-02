/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthCheckMacros.h"

LArCondFlatBase::LArCondFlatBase (const std::string& name) :
  AthMessaging(Athena::getMessageSvc(), name),
  m_isInitialized(false),
  m_onlineHelper(NULL)
{
}

LArCondFlatBase::~LArCondFlatBase() {
}
  
StatusCode LArCondFlatBase::initializeBase() {

  ATH_MSG_DEBUG( "initializeBase " );

  if (m_isInitialized) {
    ATH_MSG_DEBUG( "already initialized - returning " );
    return (StatusCode::SUCCESS);
  }
  //Get LArOnlineID....
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StoreGateSvc* detStore;
  ATH_CHECK_WITH_CONTEXT( svcLoc->service("DetectorStore",detStore), "LArCondFlatBase" );
  ATH_CHECK_WITH_CONTEXT( detStore->retrieve(m_onlineHelper,"LArOnlineID"), "LArCondFlatBase" );

  m_isInitialized = true;
  ATH_MSG_DEBUG( "end initializeBase ");
  return (StatusCode::SUCCESS);
}
