/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHisto1DCnv.h"
#include "TrigInDetEventTPCnv/TrigHisto1DCnv_p1.h"

TrigHisto1DCnv::TrigHisto1DCnv(ISvcLocator* svcloc): TrigHisto1DCnvBase(svcloc) {}
TrigHisto1DCnv::~TrigHisto1DCnv(){}

TrigHisto1D_p1* TrigHisto1DCnv::createPersistent(TrigHisto1D* transObj)  {
  MsgStream mlog(messageService(), "TrigHisto1DConverter" );
  mlog << MSG::DEBUG << "TrigHisto1DCnv::createPersistent" << endreq;
  
  TrigHisto1DCnv_p1 converter;
  TrigHisto1D_p1 *persObj = converter.createPersistent( transObj, mlog );
  
  return persObj;
}

TrigHisto1D* TrigHisto1DCnv::createTransient() {
  MsgStream mlog(messageService(), "TrigHisto1DConverter" );
  mlog << MSG::DEBUG << "TrigHisto1DCnv::createTransient " << endreq;
 
  static pool::Guid p1_guid("C304621E-2266-4863-9B47-7C3450B00399");

  TrigHisto1D *transObj = 0;
  
  if( compareClassGuid(p1_guid) ) {   
    mlog << MSG::DEBUG << "TrigHisto1DCnv::reading p1 persistent object" << endreq;
    std::auto_ptr<TrigHisto1D_p1> pers_ptr( poolReadObject<TrigHisto1D_p1>() );
    TrigHisto1DCnv_p1 converter;
    transObj = converter.createTransient(pers_ptr.get(), mlog);
  }
  else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  
  return transObj;
}
