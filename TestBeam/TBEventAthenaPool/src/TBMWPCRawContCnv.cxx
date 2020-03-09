/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBMWPCRawContCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "AthenaKernel/StorableConversions.h"


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
  CHECK(AthenaPoolConverter::initialize());

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBMWPCRawContCnv");
  log << MSG::INFO << "initialize()" << endmsg;

  return StatusCode::SUCCESS;
}


StatusCode TBMWPCRawContCnv::PoolToDataObject(DataObject*& pObj,
                                              const Token* token,
                                              const std::string& key)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(msgSvc(), "TBMWPCRawContCnv::PoolToDataObject" );
   
  StatusCode sc = TBMWPCRawContCnvBase::PoolToDataObject(pObj, token, key);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endmsg;
  }
    
  // Convert DataObject pointer to TBMWPCRawCont*
  TBMWPCRawCont* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBMWPCRawCont " << endmsg ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}
