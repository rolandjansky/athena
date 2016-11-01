/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloShowerContainerCnv.h"
#include "CaloTPCnv/CaloShowerContainer_p1.h"
#include "CaloTPCnv/CaloShowerContainer_p2.h"
#define private public
#include "CaloEvent/CaloShowerContainer.h"
#undef private
CaloShowerContainerCnv::CaloShowerContainerCnv(ISvcLocator* svcLoc) : 
  CaloShowerContainerCnvBase(svcLoc),
  m_p0_guid("CE47AFED-EC28-43BA-86C6-721B11D5B24E"),
  m_p1_guid("E632DE88-A3A5-469B-AD1F-677EA781F6C9"),
  m_p2_guid("5A38F895-CE73-456B-8FF5-4E09DC7718D5")
{}


CaloShowerContainerPERS* CaloShowerContainerCnv::createPersistent(CaloShowerContainer* trans) {
    MsgStream log(msgSvc(), "CaloShowerContainerCnv");
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Writing CaloShowerContainer_p2" << endmsg;
    CaloShowerContainerPERS* pers=new CaloShowerContainerPERS();
    m_converter2.transToPers(trans,pers,log); 
    return pers;
}
    


CaloShowerContainer* CaloShowerContainerCnv::createTransient() {
   MsgStream log(msgSvc(), "CaloShowerContainerCnv" );
   if (compareClassGuid(m_p0_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read version p0 of CaloShowerContainer. GUID=" 
	 << m_classID.toString() << endmsg;
     return poolReadObject<CaloShowerContainer>();
   }
   else if (compareClassGuid(m_p1_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Reading CaloShowerContainer_p1. GUID=" 
	 << m_classID.toString() << endmsg;
     CaloShowerContainer_p1* pers=poolReadObject<CaloShowerContainer_p1>();
     CaloShowerContainer* trans=new CaloShowerContainer();
     m_converter1.persToTrans(pers,trans,log);
     delete pers;
     return trans;
   }
   else if (compareClassGuid(m_p2_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Reading CaloShowerContainer_p2. GUID=" 
	 << m_classID.toString() << endmsg;
     CaloShowerContainer_p2* pers=poolReadObject<CaloShowerContainer_p2>();
     CaloShowerContainer* trans=new CaloShowerContainer();
     m_converter2.persToTrans(pers,trans,log);
     delete pers;
     return trans;
   }

   log << MSG::ERROR << "Unsupported persistent version of CaloShowerContainer. GUID="
       << m_classID.toString() << endmsg;
   throw std::runtime_error("Unsupported persistent version of Data Collection");
   return 0;
}
