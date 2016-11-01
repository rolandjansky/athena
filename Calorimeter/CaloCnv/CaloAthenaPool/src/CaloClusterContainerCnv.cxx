/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloClusterContainerCnv.h"

#include "CaloUtils/CaloClusterSignalState.h"


//Goes to .h file


CaloClusterContainerCnv::CaloClusterContainerCnv(ISvcLocator* svcLoc) : 
  CaloClusterContainerCnvBase(svcLoc),
  m_p0_guid("6969EDA6-698A-45AF-95A2-5836F6C1FE99"),
  m_p1_guid("89DA146E-BB20-4E35-A13E-2F64AAE97407"),
  m_p2_guid("4132E08D-4110-4242-B188-84F2C46EC0D2"),
  m_p3_guid("88F98AE8-0D4A-4BFA-94BE-FDE312E21DC9"),
  m_p4_guid("330CE691-7151-46DC-892B-46176619540C"),
  m_p5_guid("2E505417-8246-4A1E-B35F-704663C530FD"),
  m_p6_guid("57D9916D-4B2E-4F2A-9FE3-30A8F5DA3457"),
  m_p7_guid("A2739D1D-C338-41D1-94DD-A173B921DC43")
{}


CaloClusterContainerPERS* CaloClusterContainerCnv::createPersistent(CaloClusterContainer* trans) {
    MsgStream log(msgSvc(), "CaloClusterContainerCnv");
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Writing CaloClusterContainer_p7" << endmsg;
    CaloClusterContainerPERS* pers=new CaloClusterContainerPERS();
    m_converter_p7.transToPers(trans,pers,log); 
    return pers;
}
    


CaloClusterContainer* CaloClusterContainerCnv::createTransient() {
   MsgStream log(msgSvc(), "CaloClusterContainerCnv" );
   // Search for persistent version to convert - last version first
   CaloClusterContainer* trans = 0;
   if (compareClassGuid(m_p7_guid)) {
     trans=new CaloClusterContainer(SG::VIEW_ELEMENTS);
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloClusterContainer_p7. GUID=" << m_classID.toString() << endmsg; 
     CaloClusterContainer_p7* pers=poolReadObject<CaloClusterContainer_p7>();
     m_converter_p7.persToTrans(pers,trans,log);
     delete pers;
   }
   else if (compareClassGuid(m_p6_guid)) {
     trans=new CaloClusterContainer(SG::VIEW_ELEMENTS);
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloClusterContainer_p6. GUID=" << m_classID.toString() << endmsg; 
     CaloClusterContainer_p6* pers=poolReadObject<CaloClusterContainer_p6>();
     m_converter_p6.persToTrans(pers,trans,log);
     delete pers;
   }
   else if (compareClassGuid(m_p5_guid)) {
     trans=new CaloClusterContainer(SG::VIEW_ELEMENTS);
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloClusterContainer_p5. GUID=" << m_classID.toString() << endmsg;
     CaloClusterContainer_p5* pers=poolReadObject<CaloClusterContainer_p5>();
     m_converter_p5.persToTrans(pers,trans,log);
     delete pers;
   }
   else if (compareClassGuid(m_p4_guid)) {
     trans=new CaloClusterContainer(SG::VIEW_ELEMENTS);
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloClusterContainer_p4. GUID=" 
	 << m_classID.toString() << endmsg;
     CaloClusterContainer_p4* pers=poolReadObject<CaloClusterContainer_p4>();
     m_converter_p4.persToTrans(pers,trans,log);
     delete pers;
   }
   else if (compareClassGuid(m_p3_guid)) {
     trans=new CaloClusterContainer(SG::VIEW_ELEMENTS);
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloClusterContainer_p3. GUID=" 
	 << m_classID.toString() << endmsg;
     CaloClusterContainer_p3* pers=poolReadObject<CaloClusterContainer_p3>();
     m_converter_p3.persToTrans(pers,trans,log);
     delete pers;
   }
   else if (compareClassGuid(m_p2_guid)) {
     trans=new CaloClusterContainer(SG::VIEW_ELEMENTS);
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloClusterContainer_p2. GUID=" 
	 << m_classID.toString() << endmsg;
     CaloClusterContainer_p2* pers=poolReadObject<CaloClusterContainer_p2>();
     m_converter_p2.persToTrans(pers,trans,log);
     delete pers;
   }
   else if (compareClassGuid(m_p1_guid)) {
     trans=new CaloClusterContainer();
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read CaloClusterContainer_p1. GUID=" 
	 << m_classID.toString() << endmsg;
     CaloClusterContainer_p1* pers=poolReadObject<CaloClusterContainer_p1>();
     m_converter_p1.persToTrans(pers,trans,log);
     delete pers;
   }
   else if (compareClassGuid(m_p0_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read version p0 of CaloClusterContainer. GUID=" 
	 << m_classID.toString() << endmsg;
     trans =  poolReadObject<CaloClusterContainer>();
   }
   else {
     log << MSG::ERROR << "Unsupported persistent version of CaloClusterContainer GUID=" 
	 << m_classID.toString() << endmsg;
     throw std::runtime_error("Unsupported persistent version of CaloClusterContainer");
   }
   //CaloClusterSignalState::setCollectionDefaultState(trans,P4SignalState::CALIBRATED);
   for (auto clu : *trans) {
     clu->setDefaultSignalState(P4SignalState::CALIBRATED);
   }
   return trans;
}
