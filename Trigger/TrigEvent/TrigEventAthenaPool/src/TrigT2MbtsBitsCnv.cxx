/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MbtsBitsCnv.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p2.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p2.h"

TrigT2MbtsBitsCnv::TrigT2MbtsBitsCnv(ISvcLocator* svcloc): TrigT2MbtsBitsCnvBase(svcloc) {}
TrigT2MbtsBitsCnv::~TrigT2MbtsBitsCnv(){}

TrigT2MbtsBits_PERS* TrigT2MbtsBitsCnv::createPersistent(TrigT2MbtsBits* transObj)  {
  MsgStream mlog(messageService(), "TrigT2MbtsBitsConverter" );
  mlog << MSG::DEBUG << "TrigT2MbtsBitsCnv::createPersistent" << endreq;
  
  TrigT2MbtsBitsCnv_p2  converter;
  TrigT2MbtsBits_PERS *persObj = converter.createPersistent( transObj, mlog );
  
  return persObj;
}

TrigT2MbtsBits* TrigT2MbtsBitsCnv::createTransient() {
  MsgStream mlog(messageService(), "TrigT2MbtsBitsConverter" );
  mlog << MSG::DEBUG << "TrigT2MbtsBitsCnv::createTransient " << endreq;
  
  static pool::Guid p2_guid("6D130C0D-20A2-4304-8E70-6775892217A6");
  static pool::Guid p1_guid("32E86328-15EB-42F5-A93F-6F891220A626");
  
  
  TrigT2MbtsBits *transObj = 0;

  if( compareClassGuid(p2_guid) ) {   
    mlog << MSG::DEBUG << "TrigT2MbtsBitsCnv::reading p2 persistent object" << endreq;
    std::auto_ptr< TrigT2MbtsBits_p2 > pers_ptr( poolReadObject< TrigT2MbtsBits_p2 >() );
    TrigT2MbtsBitsCnv_p2 converter;
    transObj = converter.createTransient(pers_ptr.get(), mlog);
  }
  else if( compareClassGuid(p1_guid) ) {   
    mlog << MSG::DEBUG << "TrigT2MbtsBitsCnv::reading p1 persistent object" << endreq;
    std::auto_ptr< TrigT2MbtsBits_p1 > pers_ptr( poolReadObject< TrigT2MbtsBits_p1 >() );
    TrigT2MbtsBitsCnv_p1 converter;
    transObj = converter.createTransient(pers_ptr.get(), mlog);
  }
  else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  
  return transObj;
}
