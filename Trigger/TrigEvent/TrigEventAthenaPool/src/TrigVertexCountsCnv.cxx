/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigVertexCountsCnv.h"
#include "TrigInDetEventTPCnv/TrigVertexCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCnv_p1.h"

TrigVertexCountsCnv::TrigVertexCountsCnv(ISvcLocator* svcloc): TrigVertexCountsCnvBase(svcloc) {}
TrigVertexCountsCnv::~TrigVertexCountsCnv(){}

TrigVertexCounts_PERS* TrigVertexCountsCnv::createPersistent(TrigVertexCounts* transObj)  {
  MsgStream mlog(messageService(), "TrigVertexCountsConverter" );
  mlog << MSG::DEBUG << "TrigVertexCountsCnv::createPersistent" << endreq;
  
  TrigVertexCountsCnv_p1  converter;
  TrigVertexCounts_PERS *persObj = converter.createPersistent( transObj, mlog );
  
  return persObj;
}

TrigVertexCounts* TrigVertexCountsCnv::createTransient() {
  MsgStream mlog(messageService(), "TrigVertexCountsConverter" );
  mlog << MSG::DEBUG << "TrigVertexCountsCnv::createTransient " << endreq;
  
  static pool::Guid p1_guid("33E83FBA-83F1-4DC5-87BE-81A09D0FD8F8");
  
  
  TrigVertexCounts *transObj = 0;

  if( compareClassGuid(p1_guid) ) {   
    mlog << MSG::DEBUG << "TrigVertexCountsCnv::reading p1 persistent object" << endreq;
    std::auto_ptr< TrigVertexCounts_p1 > pers_ptr( poolReadObject< TrigVertexCounts_p1 >() );
    TrigVertexCountsCnv_p1 converter;
    transObj = converter.createTransient(pers_ptr.get(), mlog);
  }
  else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  
  return transObj;
}
