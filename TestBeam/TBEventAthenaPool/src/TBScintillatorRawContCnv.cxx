/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBScintillatorRawContCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Athena
#include "SGTools/StorableConversions.h"


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
  AthenaPoolConverter::initialize();

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), "TBScintillatorRawContCnv");
  log << MSG::INFO << "initialize()" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode TBScintillatorRawContCnv::PoolToDataObject(DataObject*& pObj, const Token* token)
{
  // First call base class converter to get DataObject from
  // pool. Then modify as appropriate

  MsgStream log(messageService(), "TBScintillatorRawContCnv::PoolToDataObject" );
   
  StatusCode sc = TBScintillatorRawContCnvBase::PoolToDataObject(pObj, token);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to get object from pool" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found DataObject " << endreq;
  }
    
  // Convert DataObject pointer to TBScintillatorRawCont*
  TBScintillatorRawCont* obj=0;
  SG::fromStorable(pObj, obj );
  if(!obj) {
    log << MSG::ERROR << "  failed to cast to TBScintillatorRawCont " << endreq ; 
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS; 
}

StatusCode TBScintillatorRawContCnv::DataObjectToPool(DataObject* pObj, const std::string &tname) 
{
  MsgStream log(messageService(),"TBScintillatorRawContCnv::DataObjectToPool" );
  
  return TBScintillatorRawContCnvBase::DataObjectToPool( pObj, tname) ;
}
