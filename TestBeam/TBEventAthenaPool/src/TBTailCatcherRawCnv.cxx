/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTailCatcherRawCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "AthenaKernel/StorableConversions.h"


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
  CHECK(AthenaPoolConverter::initialize());

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBTailCatcherRawCnv");
  log << MSG::INFO << "initialize()" << endmsg;

  return StatusCode::SUCCESS;
}


StatusCode TBTailCatcherRawCnv::PoolToDataObject(DataObject*& pObj,
                                                 const Token* token,
                                                 const std::string& key)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(msgSvc(), "TBTailCatcherRawCnv::PoolToDataObject" );
   
  StatusCode sc = TBTailCatcherRawCnvBase::PoolToDataObject(pObj, token, key);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endmsg;
  }
    
  // Convert DataObject pointer to TBTailCatcherRaw*
  TBTailCatcherRaw* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBTailCatcherRaw " << endmsg ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}
