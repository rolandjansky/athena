/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/CheckLArFebHeader.h"
#include "GaudiKernel/AlgFactory.h"
#include <vector>
#include "GaudiKernel/IToolSvc.h"
#include "LArRawEvent/LArFebHeaderContainer.h"

CheckLArFebHeader::CheckLArFebHeader(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_storeGateSvc(0),
    m_onlineHelper(0)
{
  m_count=0;
  //declareProperty("KeyList",m_keylistproperty);
}

CheckLArFebHeader::~CheckLArFebHeader() 
{
}

StatusCode CheckLArFebHeader::initialize()
{ 
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize" << endreq;
  StatusCode sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) 
    {log << MSG::FATAL << " Cannot locate StoreGateSvc " << std::endl;
     return StatusCode::FAILURE;
    } 

  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) 
    {log << MSG::FATAL << " Cannot locate DetectorStore " << std::endl;
     return StatusCode::FAILURE;
    } 
  sc = detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  m_count=0;  
  log << MSG::DEBUG << "======== CheckLArFebHeader initialize successfully ========" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode CheckLArFebHeader::execute()
{
  MsgStream log(msgSvc(), name());
  StatusCode sc; 
  m_count++; 
  const LArFebHeaderContainer *larFebHeaderContainer;
  sc= m_storeGateSvc->retrieve(larFebHeaderContainer);
  if (sc.isFailure() || !larFebHeaderContainer) {
    log << MSG::DEBUG << "Cannot read LArFebHeaderContainer from StoreGate!" << endreq;
    return StatusCode::FAILURE;
  }
  
  LArFebHeaderContainer::const_iterator it=larFebHeaderContainer->begin();
  LArFebHeaderContainer::const_iterator it_e=larFebHeaderContainer->end();
  for (;it!=it_e;it++) {
    if((*it)->CheckErrorELVL1Id() || (*it)->CheckErrorBCId()) {
      HWIdentifier febid=(*it)->FEBId();
      int barrel_ec = m_onlineHelper->barrel_ec(febid);
      int pos_neg   = m_onlineHelper->pos_neg(febid);
      int FT        = m_onlineHelper->feedthrough(febid);
      int slot      = m_onlineHelper->slot(febid);
      int BCID      = (*it)->BCId();
      int LVL1ID    = (*it)->ELVL1Id();
      int FebBCID   = (*it)->FebBCId();
      int FebLVL1ID = (*it)->FebELVL1Id();
	
      log << MSG::FATAL << "TTC information mismatch in event " << m_count << ":" << endreq;
      log << MSG::FATAL << "    FEBID = " << febid.get_compact() << endreq;
      log << MSG::FATAL << "      BARREL/EC = " << barrel_ec << " POS/NEG = " << pos_neg << endreq;
      log << MSG::FATAL << "      FT = " << FT << " SLOT = " << slot << endreq;
      log << MSG::FATAL << "    TTC from ROD: LVL1ID = " << LVL1ID << " BCID = " << BCID << endreq;
      log << MSG::FATAL << "    TTC from FEB: LVL1ID = " << FebLVL1ID << " BCID = " << FebBCID << endreq;
	
      return StatusCode::FAILURE;
    } // End if 
    else
      log << MSG::DEBUG << "FEB header consistent." << endreq;
  } // End FebHeader loop

  
 return StatusCode::SUCCESS;
}

StatusCode CheckLArFebHeader::finalize()
{ 
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << ">>> Finalize" << endreq;
  return StatusCode::SUCCESS;
}
