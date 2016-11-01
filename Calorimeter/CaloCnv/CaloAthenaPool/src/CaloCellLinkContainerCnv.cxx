/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellLinkContainerCnv.h"
#include "CaloTPCnv/CaloCellLinkContainer_p1.h"
#include "CaloTPCnv/CaloCellLinkContainer_p2.h"
//#define private public
#include "CaloEvent/CaloCellLinkContainer.h"
//#undef private
CaloCellLinkContainerCnv::CaloCellLinkContainerCnv(ISvcLocator* svcLoc) : 
  CaloCellLinkContainerCnvBase(svcLoc),
  m_p0_guid("6DE36687-81DE-4264-8857-41AFA0C55415"),
  m_p1_guid("D91CAA94-014D-469B-9991-2F7A53D657C5"),
  m_p2_guid("826FC299-E988-4A07-926F-7D016F62FE5D")
{}


CaloCellLinkContainerPERS* CaloCellLinkContainerCnv::createPersistent(CaloCellLinkContainer* trans) {
    MsgStream log(msgSvc(), "CaloCellLinkContainerCnv");
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Writing CaloCellLinkContainer_p1" << endmsg;
    CaloCellLinkContainerPERS* pers=new CaloCellLinkContainerPERS();
    m_converter_p2.transToPers(trans,pers,log); 
    return pers;
}
    


CaloCellLinkContainer* CaloCellLinkContainerCnv::createTransient() {
   MsgStream log(msgSvc(), "CaloCellLinkContainerCnv" );
   if (compareClassGuid(m_p0_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read version p0 of CaloCellLinkContainer. GUID=" 
	 << m_classID.toString() << endmsg;
     return poolReadObject<CaloCellLinkContainer>();
   }
   else if (compareClassGuid(m_p1_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloCellLinkContainer_p1. GUID=" 
	 << m_classID.toString() << endmsg;
     CaloCellLinkContainer_p1* pers=poolReadObject<CaloCellLinkContainer_p1>();
     CaloCellLinkContainer* trans=new CaloCellLinkContainer();
     m_converter_p1.persToTrans(pers,trans,log);
     delete pers;
     return trans;
   }
   else if (compareClassGuid(m_p2_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloCellLinkContainer_p2. GUID=" 
	 << m_classID.toString() << endmsg;
     CaloCellLinkContainer_p2* pers=poolReadObject<CaloCellLinkContainer_p2>();
     CaloCellLinkContainer* trans=new CaloCellLinkContainer();
     m_converter_p2.persToTrans(pers,trans,log);
     delete pers;
     return trans;
   }

   log << MSG::ERROR << "Unsupported persistent version of CaloCellLinkContainer. GUID=" 
       << m_classID.toString() << endmsg;
   throw std::runtime_error("Unsupported persistent version of Data Collection");
   return 0;
}
