/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBBPCRawContCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "SGTools/StorableConversions.h"


// Constructor - call base constructor and initialize local attributes
TBBPCRawContCnv::TBBPCRawContCnv(ISvcLocator* svcloc)
  :
  // Base class constructor
  TBBPCRawContCnvBase::T_AthenaPoolCnv(svcloc)
{}

// Destructor
TBBPCRawContCnv::~TBBPCRawContCnv()
{}

StatusCode TBBPCRawContCnv::initialize()
{
  // Call base clase initialize
  AthenaPoolConverter::initialize();

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBBPCRawContCnv");
  log << MSG::INFO << "initialize()" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode TBBPCRawContCnv::PoolToDataObject(DataObject*& pObj, const Token* token)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(messageService(), "TBBPCRawContCnv::PoolToDataObject" );
   
  StatusCode sc = TBBPCRawContCnvBase::PoolToDataObject(pObj, token);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endreq;
  }
    
  // Convert DataObject pointer to TBBPCRawCont*
  TBBPCRawCont* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBBPCRawCont " << endreq ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}

StatusCode TBBPCRawContCnv::DataObjectToPool(DataObject* pObj, const std::string &tname) 
{
  MsgStream log(messageService(),"TBBPCRawContCnv::DataObjectToPool" );
  
  return TBBPCRawContCnvBase::DataObjectToPool( pObj, tname) ;
}
