/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFeatureDetailsCnv.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p1.h"

MuonFeatureDetailsCnv::MuonFeatureDetailsCnv(ISvcLocator *svcloc): MuonFeatureDetailsCnvBase(svcloc) {}
MuonFeatureDetailsCnv::~MuonFeatureDetailsCnv(){}

//createPersistent
MuonFeatureDetails_PERS* MuonFeatureDetailsCnv::createPersistent(MuonFeatureDetails* transObj) {
    MsgStream mlog(messageService(), "MuonFeatureDetailsConverter" );

    mlog << MSG::DEBUG << "MuonFeatureDetailsCnv::createPersistent" << endreq;

    MuonFeatureDetailsCnv_p1   converter;
    MuonFeatureDetails_PERS *persObj = converter.createPersistent( transObj, mlog );

    return persObj;
}

//createTransient
MuonFeatureDetails* MuonFeatureDetailsCnv::createTransient() {
    MsgStream mlog(messageService(), "MuonFeatureDetailsConverter" );

    mlog << MSG::DEBUG << "MuonFeatureDetailsCnv::createTransient " << endreq;

    static pool::Guid p1_guid("E841B555-766B-48EF-96F8-F4BE39EE8BCB");

    if( compareClassGuid(p1_guid) ) {

      std::auto_ptr< MuonFeatureDetails_p1 > col_vect( poolReadObject< MuonFeatureDetails_p1 >() );
      MuonFeatureDetailsCnv_p1 converter;
      return converter.createTransient( col_vect.get(), mlog );
    } 
    else {
      throw std::runtime_error("MuonFeatureDetailsCnv: unsupported persistent version");
    }
}

