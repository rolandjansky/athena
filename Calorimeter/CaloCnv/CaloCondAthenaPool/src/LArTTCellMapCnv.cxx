/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTTCellMapCnv.h"


// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/StorableConversions.h"

LArTTCellMapCnv::LArTTCellMapCnv(ISvcLocator* svcloc)
    :
    // Base class constructor
    LArTTCellMapCnvBase::T_AthenaPoolCustCnv(svcloc)
{}

LArTTCellMapCnv::~LArTTCellMapCnv()
{}


StatusCode LArTTCellMapCnv::initialize()
{
   // Call base clase initialize
    if (!AthenaPoolConverter::initialize().isSuccess()) {
        return StatusCode::FAILURE;
    }

   // Get the messaging service, print where you are
   MsgStream log(msgSvc(), "LArTTCellMapCnv");
   log << MSG::INFO << "initialize()" << endreq;

   return StatusCode::SUCCESS;
}


StatusCode LArTTCellMapCnv::transToPers(LArTTCellMap* obj, LArTTCell_P*& persObj) {

   MsgStream log(msgSvc(),"LArTTCellMapCnv")  ;

   persObj = (obj->getP()) ; 

   if(persObj == 0 ) {
	log<<MSG::ERROR<<" no persistent object from LArTTCellMap " <<endreq;
	return StatusCode::FAILURE; 
   }

   log<<MSG::DEBUG<<"  LArTTCell size =  "<< persObj->m_v.size() <<endreq;
   std::cout <<"  LArTTCell_P pointer " << persObj  <<std::endl;

   return StatusCode::SUCCESS;
}

StatusCode LArTTCellMapCnv::persToTrans(LArTTCellMap*& transObj,LArTTCell_P* obj) {

  MsgStream log(msgSvc(),"LArTTCellMapCnv")  ;

  log<<MSG::DEBUG<<"  LArTTCell size =  "<< obj->m_v.size() <<endreq;

  transObj=new LArTTCellMap();
  transObj->set( *obj ) ; 

  return StatusCode::SUCCESS ;
}
