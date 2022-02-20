/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthCheckMacros.h"

LArCondSuperCellBase::LArCondSuperCellBase(const std::string& name) :
  AthMessaging(Athena::getMessageSvc(), name),
  m_isInitialized(false),
  m_scOnlineID(NULL)
{
}


LArCondSuperCellBase::~LArCondSuperCellBase() {
}
  
StatusCode LArCondSuperCellBase::initializeBase() {

  ATH_MSG_DEBUG( "initializeBase " );

  if (m_isInitialized) {
    ATH_MSG_DEBUG( "already initialized - returning " );
    return (StatusCode::SUCCESS);
  }
  //Get SuperCellID ...
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StoreGateSvc* detStore = nullptr;
  ATH_CHECK_WITH_CONTEXT( svcLoc->service("DetectorStore",detStore), "LArCondSuperCellBase" );
  ATH_CHECK_WITH_CONTEXT( detStore->retrieve( m_scOnlineID,"LArOnline_SuperCellID"), "LArCondSuperCellBase" );

  m_isInitialized = true;
  ATH_MSG_DEBUG( "end initializeBase " );
  return (StatusCode::SUCCESS);
}
