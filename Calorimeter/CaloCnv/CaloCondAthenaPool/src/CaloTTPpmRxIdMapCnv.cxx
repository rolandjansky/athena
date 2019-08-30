/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloTTPpmRxIdMapCnv.cxx
 * @brief Generated implementation file which includes header files needed by CaloTTPpmRxIdMapCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CaloTTPpmRxIdMapCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

// Athena
#include "AthenaKernel/StorableConversions.h"

CaloTTPpmRxIdMapCnv::CaloTTPpmRxIdMapCnv(ISvcLocator* svcloc)
    :
    // Base class constructor
    CaloTTPpmRxIdMapCnvBase::T_AthenaPoolCustCnv(svcloc)
{}

CaloTTPpmRxIdMapCnv::~CaloTTPpmRxIdMapCnv()
{}


StatusCode CaloTTPpmRxIdMapCnv::initialize()
{
   // Call base clase initialize
    if (!AthenaPoolConverter::initialize().isSuccess()) {
        return StatusCode::FAILURE;
    }

   // Get the messaging service, print where you are
   MsgStream log(msgSvc(), "CaloTTPpmRxIdMapCnv");
   log << MSG::INFO << "initialize()" << endmsg;

   return StatusCode::SUCCESS;
}


StatusCode CaloTTPpmRxIdMapCnv::transToPers(CaloTTPpmRxIdMap* obj, CaloTTPpmRxId_P*& persObj) {

   MsgStream log(msgSvc(),"CaloTTPpmRxIdMapCnv")  ;

   persObj = (obj->getP()) ;

   if(persObj == 0 ) {
	log<<MSG::ERROR<<" no persistent object from CaloTTPpmRxIdMap " <<endmsg;
	return StatusCode::FAILURE;
   }

   log<<MSG::DEBUG<<"  CaloTTPpmRxId size =  "<< persObj->m_v.size() <<endmsg;
   std::cout <<"  CaloTTPpmRxId_P pointer " << persObj  <<std::endl;

   return StatusCode::SUCCESS;
}

StatusCode CaloTTPpmRxIdMapCnv::persToTrans(CaloTTPpmRxIdMap*& transObj, CaloTTPpmRxId_P* obj) {

  MsgStream log(msgSvc(),"CaloTTPpmRxIdMapCnv")  ;

  log<<MSG::DEBUG<<"  CaloPpmRxId size =  "<< obj->m_v.size() <<endmsg;

  transObj=new CaloTTPpmRxIdMap();
  transObj->set( *obj ) ;

  return StatusCode::SUCCESS ;
}
