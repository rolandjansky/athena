/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigPassBitsCnv.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCnv_p1.h"


TrigPassBitsCnv::TrigPassBitsCnv(ISvcLocator *svcloc): TrigPassBitsCnvBase(svcloc) {}
TrigPassBitsCnv::~TrigPassBitsCnv(){}

//createPersistent
TrigPassBits_PERS* TrigPassBitsCnv::createPersistent(TrigPassBits* transObj) {
    MsgStream mlog(messageService(), "TrigPassBitsConverter" );

    mlog << MSG::DEBUG << "TrigPassBitsCnv::createPersistent" << endreq;

    TrigPassBitsCnv_p1   converter;
    TrigPassBits_PERS *persObj = converter.createPersistent( transObj, mlog );

    return persObj;
}

//createTransient
TrigPassBits* TrigPassBitsCnv::createTransient() {
    MsgStream mlog(messageService(), "TrigPassBitsConverter" );

    mlog << MSG::DEBUG << "TrigPassBitsCnv::createTransient " << endreq;

    static pool::Guid p1_guid("391FFE21-5D82-471E-9FFC-B77150142B8F");


    if( compareClassGuid(p1_guid) ) {

      std::auto_ptr< TrigPassBits_p1 > col_vect( poolReadObject< TrigPassBits_p1 >() );
      TrigPassBitsCnv_p1 converter;
      return converter.createTransient( col_vect.get(), mlog );

    }
    else {
      throw std::runtime_error("Unsupported persistent version");
    }
   
}
