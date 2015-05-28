/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHisto2DCnv.h"
#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"

TrigHisto2DCnv::TrigHisto2DCnv(ISvcLocator* svcloc): TrigHisto2DCnvBase(svcloc) {}
TrigHisto2DCnv::~TrigHisto2DCnv(){}

TrigHisto2D_p1* TrigHisto2DCnv::createPersistent(TrigHisto2D* transObj)  {
  MsgStream mlog(messageService(), "TrigHisto2DConverter" );
  mlog << MSG::DEBUG << "TrigHisto2DCnv::createPersistent" << endreq;
  
  TrigHisto2DCnv_p1 converter;
  TrigHisto2D_p1 *persObj = converter.createPersistent( transObj, mlog );
  
  return persObj;
}

TrigHisto2D* TrigHisto2DCnv::createTransient() {
  MsgStream mlog(messageService(), "TrigHisto2DConverter" );
  mlog << MSG::DEBUG << "TrigHisto2DCnv::createTransient " << endreq;
  
  static pool::Guid p1_guid("8CF4D0A0-2957-4014-AEB2-140D1A013E7F");

  TrigHisto2D *transObj = 0;

  if( compareClassGuid(p1_guid) ) {
    mlog << MSG::DEBUG << "TrigHisto2DCnv::reading p1 persistent object" << endreq;
    std::auto_ptr<TrigHisto2D_p1> pers_ptr( poolReadObject<TrigHisto2D_p1>() );
    TrigHisto2DCnv_p1 converter;
    transObj = converter.createTransient(pers_ptr.get(), mlog);
  }
  else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  
  return transObj;
}
