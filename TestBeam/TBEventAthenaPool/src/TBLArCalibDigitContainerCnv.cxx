/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBLArCalibDigitContainerCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "AthenaKernel/StorableConversions.h"


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
  CHECK(AthenaPoolConverter::initialize());

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBLArCalibDigitContainerCnv");
  log << MSG::INFO << "initialize()" << endmsg;

  return StatusCode::SUCCESS;
}


StatusCode TBLArCalibDigitContainerCnv::PoolToDataObject(DataObject*& pObj,
                                                         const Token* token,
                                                         const std::string& key)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(msgSvc(), "TBLArCalibDigitContainerCnv::PoolToDataObject" );
   
  StatusCode sc = TBLArCalibDigitContainerCnvBase::PoolToDataObject(pObj, token, key);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endmsg;
  }
    
  // Convert DataObject pointer to TBLArCalibDigitContainer*
  TBLArCalibDigitContainer* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBLArCalibDigitContainer " << endmsg ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}
