/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDigitContainerCnv.h"
#include "LArTPCnv/LArDigitContainer_p1.h"
//#include "LArRawEvent/LArDigitContainer.h"


LArDigitContainerCnv::LArDigitContainerCnv(ISvcLocator* svcLoc) : 
  LArDigitContainerCnvBase(svcLoc),
  m_p0_guid("B15FFDA0-206D-4062-8B5F-582A1ECD5502"),
  m_p1_guid("F1876026-CDFE-4110-AA59-E441BAA5DE44")
{}



LArDigitContainerPERS* LArDigitContainerCnv::createPersistent(LArDigitContainer* trans) {
    MsgStream log(messageService(), "LArDigitContainerCnv");
    log << MSG::DEBUG << "Writing LArDigitContainer_p2" << endreq;
    LArDigitContainerPERS* pers=new LArDigitContainerPERS();
    m_converter.transToPers(trans,pers,log); 
    return pers;
}
    


LArDigitContainer* LArDigitContainerCnv::createTransient() {
   MsgStream log(messageService(), "LArDigitContainerCnv" );
   if (compareClassGuid(m_p0_guid)) {
     log << MSG::DEBUG << "Read version p0 of LArDigitContainer. GUID=" 
	 << m_classID.toString() << endreq;
     return poolReadObject<LArDigitContainer>();
   }
   else if (compareClassGuid(m_p1_guid)) {
     log << MSG::DEBUG << "Reading LArDigitContainer_p1. GUID=" 
	 << m_classID.toString() << endreq;
     LArDigitContainer* trans=new LArDigitContainer();
     LArDigitContainer_p1* pers=poolReadObject<LArDigitContainer_p1>();
     m_converter.persToTrans(pers,trans, log);
     delete pers;
     return trans;
   }

   log << MSG::ERROR << "Unsupported persistent version of LArDigitContainer. GUID="
       << m_classID.toString() << endreq;
   throw std::runtime_error("Unsupported persistent version of Data Collection");
   // not reached
}
