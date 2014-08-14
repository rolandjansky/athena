/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTrtHitCountsCnv.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCnv_p1.h"

TrigTrtHitCountsCnv::TrigTrtHitCountsCnv(ISvcLocator* svcloc): TrigTrtHitCountsCnvBase(svcloc) {}
TrigTrtHitCountsCnv::~TrigTrtHitCountsCnv(){}

TrigTrtHitCounts_PERS* TrigTrtHitCountsCnv::createPersistent(TrigTrtHitCounts* transObj)  {
  MsgStream mlog(messageService(), "TrigTrtHitCountsConverter" );
  mlog << MSG::DEBUG << "TrigTrtHitCountsCnv::createPersistent" << endreq;
  
  TrigTrtHitCountsCnv_p1  converter;
  TrigTrtHitCounts_PERS *persObj = converter.createPersistent( transObj, mlog );
  
  return persObj;
}

TrigTrtHitCounts* TrigTrtHitCountsCnv::createTransient() {
  MsgStream mlog(messageService(), "TrigTrtHitCountsConverter" );
  mlog << MSG::DEBUG << "TrigTrtHitCountsCnv::createTransient " << endreq;
  
  static pool::Guid p1_guid("FB33CC3C-74B8-43DF-ABDA-E56BEA1D9D61");
  
  TrigTrtHitCounts *transObj = 0;

  if( compareClassGuid(p1_guid) ) {   
    mlog << MSG::DEBUG << "TrigTrtHitCountsCnv::reading p1 persistent object" << endreq;
    std::auto_ptr< TrigTrtHitCounts_p1 > pers_ptr( poolReadObject< TrigTrtHitCounts_p1 >() );
    TrigTrtHitCountsCnv_p1 converter;
    transObj = converter.createTransient(pers_ptr.get(), mlog);
  }
  else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  
  return transObj;
}
