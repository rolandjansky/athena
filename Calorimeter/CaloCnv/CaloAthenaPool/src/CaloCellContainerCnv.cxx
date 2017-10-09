/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellContainerCnv.h"
#include "CaloEvent/CaloCompactCellContainer.h"
#include "CaloEvent/CaloCellContainer.h"

// CaloDetDescr includes
#include "CaloDetDescr/CaloDetDescrManager.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

CaloCellContainerCnv::CaloCellContainerCnv(ISvcLocator* svcloc)
  : CaloCellContainerCnvBase::T_AthenaPoolCustomCnv(svcloc),
    m_p1_guid("91B7AAA5-E302-4666-A4F6-7B331240AF23")
{}

CaloCellContainerCnv::~CaloCellContainerCnv(){
	}
	

CaloCellContainerPERS* CaloCellContainerCnv::createPersistent(CaloCellContainer* trans) {
    MsgStream log(msgSvc(), "CaloCellContainerCnv");
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Writing CaloCellContainer_p1" << endmsg;
    CaloCellContainerPERS* pers=new CaloCellContainerPERS();
    m_converter1.transToPers(trans,pers); 
    return pers;
}
    


CaloCellContainer* CaloCellContainerCnv::createTransient() {
   MsgStream log(msgSvc(), "CaloCellContainerCnv" );
   CaloCellContainer* trans=new CaloCellContainer();
   if (compareClassGuid(m_p1_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Reading CaloCellContainer_p1. GUID=" << m_classID.toString() << endmsg;
     CaloCompactCellContainer* pers=poolReadObject<CaloCompactCellContainer>();
     m_converter1.persToTrans(pers,trans);
     delete pers;
     return trans;
   } else {
     log << MSG::ERROR << "Unsupported persistent version of CaloCellContainer. GUID="<< m_classID.toString() << endmsg;
     throw std::runtime_error("Unsupported persistent version of Data Collection");
   }
   return trans;
}

	
