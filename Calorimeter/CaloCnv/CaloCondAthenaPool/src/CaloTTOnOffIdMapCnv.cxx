/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloTTOnOffIdMapCnv.cxx
 * @brief Generated implementation file which includes header files needed by CaloTTOnOffIdMapCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CaloTTOnOffIdMapCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

// Athena
#include "AthenaKernel/StorableConversions.h"

CaloTTOnOffIdMapCnv::CaloTTOnOffIdMapCnv(ISvcLocator* svcloc)
    :
    // Base class constructor
    CaloTTOnOffIdMapCnvBase::T_AthenaPoolCustCnv(svcloc)
{}

CaloTTOnOffIdMapCnv::~CaloTTOnOffIdMapCnv()
{}


StatusCode CaloTTOnOffIdMapCnv::initialize()
{
   // Call base clase initialize
    if (!AthenaPoolConverter::initialize().isSuccess()) {
        return StatusCode::FAILURE;
    }

   // Get the messaging service, print where you are
   MsgStream log(msgSvc(), "CaloTTOnOffIdMapCnv");
   log << MSG::INFO << "initialize()" << endmsg;

   return StatusCode::SUCCESS;
}


StatusCode CaloTTOnOffIdMapCnv::transToPers(CaloTTOnOffIdMap* obj, CaloTTOnOffId_P*& persObj) {

   MsgStream log(msgSvc(),"CaloTTOnOffIdMapCnv")  ;

   persObj = (obj->getP()) ;

   if(persObj == 0 ) {
	log<<MSG::ERROR<<" no persistent object from CaloTTOnOffIdMap " <<endmsg;
	return StatusCode::FAILURE;
   }

   log<<MSG::DEBUG<<"  CaloTTOnOffId size =  "<< persObj->m_v.size() <<endmsg;
   std::cout <<"  CaloTTOnOffId_P pointer " << persObj  <<std::endl;

   return StatusCode::SUCCESS;
}

StatusCode CaloTTOnOffIdMapCnv::persToTrans(CaloTTOnOffIdMap*& transObj, CaloTTOnOffId_P* obj) {

  MsgStream log(msgSvc(),"CaloTTOnOffIdMapCnv")  ;

  log<<MSG::DEBUG<<"  CaloOnOffId size =  "<< obj->m_v.size() <<endmsg;

  transObj=new CaloTTOnOffIdMap();
  transObj->set( *obj ) ;

  return StatusCode::SUCCESS ;
}
