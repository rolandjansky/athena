/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigPassFlagsCnv.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCnv_p1.h"


TrigPassFlagsCnv::TrigPassFlagsCnv(ISvcLocator *svcloc): TrigPassFlagsCnvBase(svcloc) {}
TrigPassFlagsCnv::~TrigPassFlagsCnv(){}

//createPersistent
TrigPassFlags_PERS* TrigPassFlagsCnv::createPersistent(TrigPassFlags* transObj) {
    MsgStream mlog(messageService(), "TrigPassFlagsConverter" );

    if(mlog.level()<=MSG::DEBUG)
       mlog << MSG::DEBUG << "TrigPassFlagsCnv::createPersistent" << endreq;

    TrigPassFlagsCnv_p1   converter;
    TrigPassFlags_PERS *persObj = converter.createPersistent( transObj, mlog );

    return persObj;
}

//createTransient
TrigPassFlags* TrigPassFlagsCnv::createTransient() {
    MsgStream mlog(messageService(), "TrigPassFlagsConverter" );

    if(mlog.level()<=MSG::DEBUG)
       mlog << MSG::DEBUG << "TrigPassFlagsCnv::createTransient " << endreq;

    static pool::Guid p1_guid("1FA7E531-B1F9-4EC5-9BD1-FD19CE7368B6");


    if( compareClassGuid(p1_guid) ) {

      std::auto_ptr< TrigPassFlags_p1 > col_vect( poolReadObject< TrigPassFlags_p1 >() );
      TrigPassFlagsCnv_p1 converter;
      return converter.createTransient( col_vect.get(), mlog );

    }
    else {
      throw std::runtime_error("Unsupported persistent version");
    }
   
}
