/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AtlasDetDescr/AtlasDetectorID.h"

#include "LArRawUtils/LArRawDetSelector.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h" 

LArRawDetSelector::LArRawDetSelector ( const LArRawChannelContainer*  )
  : m_onlineID(0),
    m_caloCellID(0),
    m_em(false),
    m_hec(false),
    m_fcal(false)
{

  ISvcLocator* svcLoc = Gaudi::svcLocator( );

  IMessageSvc* msgSvc;
 
  StatusCode sc=svcLoc->service("MessageSvc", msgSvc);
  if (sc.isFailure()) {
    std::cerr << "LArRawDetSelector: Could not get MessageSvc!" << std::endl; 
    return;
  }

  MsgStream log(msgSvc,"LArRawDetSelector");

  StoreGateSvc* sg; 
  if(svcLoc->service( "DetectorStore",sg).isFailure()) {
    log << MSG::ERROR << "Faild to get DetectorStore" << endreq;
    return;
  }
    if(sg->retrieve(m_onlineID).isFailure()) {
      log << MSG::ERROR << "Faild to get LArOnlineID helper" << endreq;
      return;
    }
    if(sg->retrieve(m_caloCellID).isFailure()) {
      log << MSG::ERROR << "Faild to get LArOnlineID helper" << endreq;
      return;
    }
    return ;
}

void LArRawDetSelector::setDet(const Identifier& id ){

  m_em  = m_caloCellID->is_lar_em(id) ;
  m_hec  = m_caloCellID->is_lar_hec(id) ;
  m_fcal  = m_caloCellID->is_lar_fcal(id) ;
  
return; 

}

void LArRawDetSelector::setDet(const HWIdentifier& chid ){

  m_em    = m_onlineID->isEMBchannel(chid) || m_onlineID->isEMECchannel(chid);
  m_hec   = m_onlineID->isHECchannel(chid);
  m_fcal  = m_onlineID->isFCALchannel(chid);
  
return; 

}



bool LArRawDetSelector::select(const LArRawChannel* rc){

  const HWIdentifier chid=rc->identify();

  if(m_em && (m_onlineID->isEMBchannel(chid) || m_onlineID->isEMECchannel(chid))) return true;

  if(m_hec && m_onlineID->isHECchannel(chid)) return true;

  if(m_fcal && m_onlineID->isFCALchannel(chid)) return true;

  return false;
}

