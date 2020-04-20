/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBScintillatorRawContCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "AthenaKernel/StorableConversions.h"


// Constructor - call base constructor and initialize local attributes
TBScintillatorRawContCnv::TBScintillatorRawContCnv(ISvcLocator* svcloc)
  :
  // Base class constructor
  TBScintillatorRawContCnvBase::T_AthenaPoolCnv(svcloc)
{}

// Destructor
TBScintillatorRawContCnv::~TBScintillatorRawContCnv()
{}

StatusCode TBScintillatorRawContCnv::initialize()
{
  // Call base clase initialize
  CHECK(AthenaPoolConverter::initialize());

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBScintillatorRawContCnv");
  log << MSG::INFO << "initialize()" << endmsg;

  return StatusCode::SUCCESS;
}


StatusCode TBScintillatorRawContCnv::PoolToDataObject(DataObject*& pObj,
                                                      const Token* token,
                                                      const std::string& key)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(msgSvc(), "TBScintillatorRawContCnv::PoolToDataObject" );
   
  StatusCode sc = TBScintillatorRawContCnvBase::PoolToDataObject(pObj, token, key);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endmsg;
  }
    
  // Convert DataObject pointer to TBScintillatorRawCont*
  TBScintillatorRawCont* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBScintillatorRawCont " << endmsg ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}
