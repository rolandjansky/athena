/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBHitPlaneContCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "SGTools/StorableConversions.h"


// Constructor - call base constructor and initialize local attributes
TBHitPlaneContCnv::TBHitPlaneContCnv(ISvcLocator* svcloc)
  :
  // Base class constructor
  TBHitPlaneContCnvBase::T_AthenaPoolCnv(svcloc)
{}

// Destructor
TBHitPlaneContCnv::~TBHitPlaneContCnv()
{}

StatusCode TBHitPlaneContCnv::initialize()
{
  // Call base clase initialize
  AthenaPoolConverter::initialize();

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBHitPlaneContCnv");
  log << MSG::INFO << "initialize()" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode TBHitPlaneContCnv::PoolToDataObject(DataObject*& pObj, const Token* token)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(messageService(), "TBHitPlaneContCnv::PoolToDataObject" );
   
  StatusCode sc = TBHitPlaneContCnvBase::PoolToDataObject(pObj, token);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endreq;
  }
    
  // Convert DataObject pointer to TBHitPlaneCont*
  TBHitPlaneCont* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBHitPlaneCont " << endreq ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}

StatusCode TBHitPlaneContCnv::DataObjectToPool(DataObject* pObj, const std::string &tname) 
{
  MsgStream log(messageService(),"TBHitPlaneContCnv::DataObjectToPool" );
  
  return TBHitPlaneContCnvBase::DataObjectToPool( pObj, tname) ;
}
