/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBMWPCRawContCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "SGTools/StorableConversions.h"


// Constructor - call base constructor and initialize local attributes
TBMWPCRawContCnv::TBMWPCRawContCnv(ISvcLocator* svcloc)
  :
  // Base class constructor
  TBMWPCRawContCnvBase::T_AthenaPoolCnv(svcloc)
{}

// Destructor
TBMWPCRawContCnv::~TBMWPCRawContCnv()
{}

StatusCode TBMWPCRawContCnv::initialize()
{
  // Call base clase initialize
  AthenaPoolConverter::initialize();

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBMWPCRawContCnv");
  log << MSG::INFO << "initialize()" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode TBMWPCRawContCnv::PoolToDataObject(DataObject*& pObj, const Token* token)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(messageService(), "TBMWPCRawContCnv::PoolToDataObject" );
   
  StatusCode sc = TBMWPCRawContCnvBase::PoolToDataObject(pObj, token);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endreq;
  }
    
  // Convert DataObject pointer to TBMWPCRawCont*
  TBMWPCRawCont* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBMWPCRawCont " << endreq ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}

StatusCode TBMWPCRawContCnv::DataObjectToPool(DataObject* pObj, const std::string &tname) 
{
  MsgStream log(messageService(),"TBMWPCRawContCnv::DataObjectToPool" );
  
  return TBMWPCRawContCnvBase::DataObjectToPool( pObj, tname) ;
}
