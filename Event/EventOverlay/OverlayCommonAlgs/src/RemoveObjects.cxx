/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Ketevi A. Assamagan, October 2009

#include "RemoveObjects.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"

#include <iostream>
#include <typeinfo>

//================================================================
RemoveObjects::RemoveObjects(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator)
{
  declareProperty("RemoveTileMC", m_removeTileMC=false);
  declareProperty("RemoveLArMC", m_removeLArMC=false);
  declareProperty("RemoveTriggerTowerMC", m_removeTriggerTowerMC=false);
}

//================================================================
StatusCode RemoveObjects::overlayInitialize()
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RemoveObjects::overlayFinalize() 
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RemoveObjects::overlayExecute() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "RemoveObjects::execute() begin"<< endmsg;
  
  if ( m_removeTileMC ) {
     removeAllObjectsOfType<TileDigitsContainer>(&*m_storeGateMC);
     removeAllObjectsOfType<TileRawChannelContainer>(&*m_storeGateMC);
  }

  if ( m_removeLArMC ) {
     removeAllObjectsOfType<LArDigitContainer>(&*m_storeGateMC);
     //log << MSG::INFO << "LAr objects removed" << endmsg;
     //removeAllObjectsOfType<LArRawChannelContainer>(&*m_storeGateMC);
  }

  if ( m_removeTriggerTowerMC ) {
     removeAllObjectsOfType<TriggerTowerCollection>(&*m_storeGateMC);
  }

  log << MSG::DEBUG << "RemoveObjects::execute() end"<< endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
