/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBLArCalibDigitContainerCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "SGTools/StorableConversions.h"


// Constructor - call base constructor and initialize local attributes
TBLArCalibDigitContainerCnv::TBLArCalibDigitContainerCnv(ISvcLocator* svcloc)
  :
  // Base class constructor
  TBLArCalibDigitContainerCnvBase::T_AthenaPoolCnv(svcloc)
{}

// Destructor
TBLArCalibDigitContainerCnv::~TBLArCalibDigitContainerCnv()
{}

StatusCode TBLArCalibDigitContainerCnv::initialize()
{
  // Call base clase initialize
  AthenaPoolConverter::initialize();

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBLArCalibDigitContainerCnv");
  log << MSG::INFO << "initialize()" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode TBLArCalibDigitContainerCnv::PoolToDataObject(DataObject*& pObj, const Token* token)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(messageService(), "TBLArCalibDigitContainerCnv::PoolToDataObject" );
   
  StatusCode sc = TBLArCalibDigitContainerCnvBase::PoolToDataObject(pObj, token);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endreq;
  }
    
  // Convert DataObject pointer to TBLArCalibDigitContainer*
  TBLArCalibDigitContainer* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBLArCalibDigitContainer " << endreq ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}

StatusCode TBLArCalibDigitContainerCnv::DataObjectToPool(DataObject* pObj, const std::string &tname) 
{
  MsgStream log(messageService(),"TBLArCalibDigitContainerCnv::DataObjectToPool" );
  
  return TBLArCalibDigitContainerCnvBase::DataObjectToPool( pObj, tname) ;
}
