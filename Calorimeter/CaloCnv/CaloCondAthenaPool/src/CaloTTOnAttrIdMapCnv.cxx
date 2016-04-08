/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloTTOnAttrIdMapCnv.cxx
 * @brief Generated implementation file which includes header files needed by CaloTTOnAttrIdMapCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CaloTTOnAttrIdMapCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/StorableConversions.h"

CaloTTOnAttrIdMapCnv::CaloTTOnAttrIdMapCnv(ISvcLocator* svcloc)
    :
    // Base class constructor
    CaloTTOnAttrIdMapCnvBase::T_AthenaPoolCustCnv(svcloc)
{}

CaloTTOnAttrIdMapCnv::~CaloTTOnAttrIdMapCnv()
{}


StatusCode CaloTTOnAttrIdMapCnv::initialize()
{
   // Call base clase initialize
    if (!AthenaPoolConverter::initialize().isSuccess()) {
        return StatusCode::FAILURE;
    }

   // Get the messaging service, print where you are
   MsgStream log(msgSvc(), "CaloTTOnAttrIdMapCnv");
   log << MSG::INFO << "initialize()" << endreq;

   return StatusCode::SUCCESS;
}


StatusCode CaloTTOnAttrIdMapCnv::transToPers(CaloTTOnAttrIdMap* obj, CaloTTOnAttrId_P*& persObj) {

   MsgStream log(msgSvc(),"CaloTTOnAttrIdMapCnv")  ;

   persObj = (obj->getP()) ;

   if(persObj == 0 ) {
	log<<MSG::ERROR<<" no persistent object from TTOnAttrIdMap " <<endreq;
	return StatusCode::FAILURE;
   }

   log<<MSG::DEBUG<<"  CaloTTOnAttrId size =  "<< persObj->m_v.size() <<endreq;
   std::cout <<"  CaloTTOnAttrId_P pointer " << persObj  <<std::endl;

   return StatusCode::SUCCESS;
}

StatusCode CaloTTOnAttrIdMapCnv::persToTrans(CaloTTOnAttrIdMap*& transObj, CaloTTOnAttrId_P* obj) {

  MsgStream log(msgSvc(),"CaloTTOnAttrIdMapCnv")  ;

  log<<MSG::DEBUG<<"  CaloOnAttrId size =  "<< obj->m_v.size() <<endreq;

  transObj=new CaloTTOnAttrIdMap();
  transObj->set( *obj ) ;

  return StatusCode::SUCCESS ;
}

