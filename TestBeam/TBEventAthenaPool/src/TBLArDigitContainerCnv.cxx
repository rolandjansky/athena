/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBLArDigitContainerCnv.h"
#include "TBTPCnv/TBLArDigitContainer_p1.h"
//#include "LArRawEvent/LArDigitContainer.h"


TBLArDigitContainerCnv::TBLArDigitContainerCnv(ISvcLocator* svcLoc) : 
  TBLArDigitContainerCnvBase(svcLoc),
  p0_guid("B15FFDA0-206D-4062-8B5F-582A1ECD5502"),	// GUID of the transient object
  p1_guid("9F58DDD2-ACDC-4ECF-A714-779B05F94649")		// GUID of the persistent object
{}



TBLArDigitContainerPERS* TBLArDigitContainerCnv::createPersistent(TBLArDigitContainer* trans) {
    MsgStream log(messageService(), "TBLArDigitContainerCnv");
    log << MSG::DEBUG << "Writing TBLArDigitContainer_p2" << endreq;
    TBLArDigitContainerPERS* pers=new TBLArDigitContainerPERS();
    m_converter.transToPers(trans,pers,log); 
    return pers;
}
    


TBLArDigitContainer* TBLArDigitContainerCnv::createTransient() {
   MsgStream log(messageService(), "TBLArDigitContainerCnv" );
   TBLArDigitContainer* trans=new TBLArDigitContainer();
   if (compareClassGuid(p0_guid)) {
     log << MSG::DEBUG << "Read version p0 of TBLArDigitContainer. GUID=" 
         << m_classID.toString() << endreq;
     return poolReadObject<TBLArDigitContainer>();
   }
   else if (compareClassGuid(p1_guid)) {
     log << MSG::DEBUG << "Reading TBLArDigitContainer_p1. GUID=" 
         << m_classID.toString() << endreq;
     TBLArDigitContainer_p1* pers=poolReadObject<TBLArDigitContainer_p1>();
     m_converter.persToTrans(pers,trans, log);
     delete pers;
     return trans;
   }
   else {
     log << MSG::ERROR << "Unsupported persistent version of TBLArDigitContainer. GUID="
     << m_classID.toString() << endreq;
     throw std::runtime_error("Unsupported persistent version of Data Collection");
   }
   return trans;
}

