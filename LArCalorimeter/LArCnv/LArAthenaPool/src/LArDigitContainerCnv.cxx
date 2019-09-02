/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDigitContainerCnv.h"
#include "LArTPCnv/LArDigitContainer_p1.h"
#include "LArTPCnv/LArDigitContainer_p2.h"
#include "LArTPCnv/LArDigitContainerCnv_p1.h"
#include "LArTPCnv/LArDigitContainerCnv_p2.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"
#include <memory>

LArDigitContainerCnv::LArDigitContainerCnv(ISvcLocator* svcLoc) : 
  LArDigitContainerCnvBase(svcLoc),
  m_p0_guid("B15FFDA0-206D-4062-8B5F-582A1ECD5502"),
  m_p1_guid("F1876026-CDFE-4110-AA59-E441BAA5DE44"),
  m_p2_guid("66F5B7Af-595C-4F79-A2B7-56590777C313")
{}
  
//StoreGateSvc* detStore=serviceLocator()->service("DetectorStore");
StatusCode LArDigitContainerCnv::initialize() {

  MsgStream log(msgSvc(), "LArDigitContainerCnv");
  StoreGateSvc *detStore=nullptr;
  StatusCode sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  } 
  const LArOnlineID* idHelper=nullptr;
  sc=detStore->retrieve(idHelper,"LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not retrieve LArOnlineID helper from Detector Store" << endmsg;
    return StatusCode::FAILURE;
  }
  m_idHelper=idHelper;//cast to base-class
  return LArDigitContainerCnvBase::initialize();
}


LArDigitContainerPERS* LArDigitContainerCnv::createPersistent(LArDigitContainer* trans) {
    MsgStream log(msgSvc(), "LArDigitContainerCnv");
    log << MSG::DEBUG << "Writing LArDigitContainer_p2" << endmsg;
    LArDigitContainerPERS* pers=new LArDigitContainerPERS();
    LArDigitContainerCnv_p2 converter(m_idHelper);
    converter.transToPers(trans,pers,log); 
    return pers;
}
    


LArDigitContainer* LArDigitContainerCnv::createTransient() {
   MsgStream log(msgSvc(), "LArDigitContainerCnv" );
   if (compareClassGuid(m_p0_guid)) {
     log << MSG::DEBUG << "Read version p0 of LArDigitContainer. GUID=" 
	 << m_classID.toString() << endmsg;
     return poolReadObject<LArDigitContainer>();
   }
   else if (compareClassGuid(m_p1_guid)) {
     log << MSG::DEBUG << "Reading LArDigitContainer_p1. GUID=" 
	 << m_classID.toString() << endmsg;
     LArDigitContainer* trans=new LArDigitContainer();
     std::unique_ptr<LArDigitContainer_p1> pers(poolReadObject<LArDigitContainer_p1>());
     LArDigitContainerCnv_p1 converter;
     converter.persToTrans(pers.get(),trans, log);
     return trans;
   } 
   else if (compareClassGuid(m_p2_guid)) {
     log << MSG::DEBUG << "Reading LArDigitContainer_p2. GUID=" 
	 << m_classID.toString() << endmsg;
     LArDigitContainer* trans=new LArDigitContainer();
     std::unique_ptr<LArDigitContainer_p2> pers(poolReadObject<LArDigitContainer_p2>());
     LArDigitContainerCnv_p2 converter(m_idHelper);
     converter.persToTrans(pers.get(),trans, log);
     return trans;
   }
   log << MSG::ERROR << "Unsupported persistent version of LArDigitContainer. GUID="
       << m_classID.toString() << endmsg;
   throw std::runtime_error("Unsupported persistent version of Data Collection");
   // not reached
}
