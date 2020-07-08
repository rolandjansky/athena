/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArLATOMEHeaderContainerCnv.h"
#include "LArTPCnv/LArLATOMEHeaderContainer_p1.h"
#include "LArTPCnv/LArLATOMEHeaderContainerCnv_p1.h"
#include "StoreGate/StoreGateSvc.h"
#include <memory>

LArLATOMEHeaderContainerCnv::LArLATOMEHeaderContainerCnv(ISvcLocator* svcLoc) : 
  LArLATOMEHeaderContainerCnvBase(svcLoc),
  m_p1_guid("7FE06234-8574-4514-86C7-1FD09E97D713")
{}
  
StatusCode LArLATOMEHeaderContainerCnv::initialize() {

  MsgStream log(msgSvc(), "LArLATOMEHeaderContainerCnv");
  StoreGateSvc *detStore=nullptr;
  StatusCode sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("DetectorStore service not found !");
    return StatusCode::FAILURE;
  } 
  return LArLATOMEHeaderContainerCnvBase::initialize();
}


LArLATOMEHeaderContainerPERS* LArLATOMEHeaderContainerCnv::createPersistent(LArLATOMEHeaderContainer* trans) {
    MsgStream log(msgSvc(), "LArLATOMEHeaderContainerCnv");
    log << MSG::DEBUG << "Writing LArDigitContainer_p2" << endmsg;
    LArLATOMEHeaderContainerPERS* pers=new LArLATOMEHeaderContainerPERS();
    LArLATOMEHeaderContainerCnv_p1 converter;
    converter.transToPers(trans,pers,log); 
    return pers;
}
    


LArLATOMEHeaderContainer* LArLATOMEHeaderContainerCnv::createTransient() {
   MsgStream log(msgSvc(), "LArLATOMEHeaderContainerCnv" );
   if (compareClassGuid(m_p1_guid)) {
     log << MSG::DEBUG << "Reading LArLATOMEHeaderContainer_p1. GUID=" 
	 << m_classID.toString() << endmsg;
     LArLATOMEHeaderContainer* trans=new LArLATOMEHeaderContainer();
     std::unique_ptr<LArLATOMEHeaderContainer_p1> pers(poolReadObject<LArLATOMEHeaderContainer_p1>());
     LArLATOMEHeaderContainerCnv_p1 converter;
     converter.persToTrans(pers.get(),trans, log);
     return trans;
   } 
   log << MSG::ERROR << "Unsupported persistent version of LArLATOMEHeaderContainer. GUID="
       << m_classID.toString() << endmsg;
   throw std::runtime_error("Unsupported persistent version of Data Collection");
   // not reached
}
