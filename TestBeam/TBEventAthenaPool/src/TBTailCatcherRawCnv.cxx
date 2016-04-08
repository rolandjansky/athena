/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTailCatcherRawCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "SGTools/StorableConversions.h"


// Constructor - call base constructor and initialize local attributes
TBTailCatcherRawCnv::TBTailCatcherRawCnv(ISvcLocator* svcloc)
  :
  // Base class constructor
  TBTailCatcherRawCnvBase::T_AthenaPoolCnv(svcloc)
{}

// Destructor
TBTailCatcherRawCnv::~TBTailCatcherRawCnv()
{}

StatusCode TBTailCatcherRawCnv::initialize()
{
  // Call base clase initialize
  AthenaPoolConverter::initialize();

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBTailCatcherRawCnv");
  log << MSG::INFO << "initialize()" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode TBTailCatcherRawCnv::PoolToDataObject(DataObject*& pObj, const Token* token)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(messageService(), "TBTailCatcherRawCnv::PoolToDataObject" );
   
  StatusCode sc = TBTailCatcherRawCnvBase::PoolToDataObject(pObj, token);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endreq;
  }
    
  // Convert DataObject pointer to TBTailCatcherRaw*
  TBTailCatcherRaw* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBTailCatcherRaw " << endreq ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}

StatusCode TBTailCatcherRawCnv::DataObjectToPool(DataObject* pObj, const std::string &tname) 
{
  MsgStream log(messageService(),"TBTailCatcherRawCnv::DataObjectToPool" );
  
  return TBTailCatcherRawCnvBase::DataObjectToPool( pObj, tname) ;
}
