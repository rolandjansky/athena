/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFeatureCnv.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p2.h"

MuonFeatureCnv::MuonFeatureCnv(ISvcLocator *svcloc): MuonFeatureCnvBase(svcloc) {}
MuonFeatureCnv::~MuonFeatureCnv(){}

//createPersistent
MuonFeature_PERS* MuonFeatureCnv::createPersistent(MuonFeature* transObj) {
    MsgStream mlog(messageService(), "MuonFeatureConverter" );

    mlog << MSG::DEBUG << "MuonFeatureCnv::createPersistent" << endreq;

    MuonFeatureCnv_p2   converter;
    MuonFeature_PERS *persObj = converter.createPersistent( transObj, mlog );

    return persObj;
}

//createTransient
MuonFeature* MuonFeatureCnv::createTransient() {
    MsgStream mlog(messageService(), "MuonFeatureConverter" );

    mlog << MSG::DEBUG << "MuonFeatureCnv::createTransient " << endreq;

    static pool::Guid p2_guid("3785A9E4-7845-4B54-B49F-DCC2A24409C0");
    static pool::Guid p1_guid("3DFFECBF-3251-4BE7-9D12-B3A9FCAC486E");
    static pool::Guid p0_guid("295FBAFB-ED82-43EA-8B63-6E3D3F4D2A9F");

    if( compareClassGuid(p1_guid) ) {

      std::auto_ptr< MuonFeature_p1 > col_vect( poolReadObject< MuonFeature_p1 >() );
      MuonFeatureCnv_p1 converter;
      return converter.createTransient( col_vect.get(), mlog );

    }
    else if( compareClassGuid(p2_guid) ) {

      std::auto_ptr< MuonFeature_p2 > col_vect( poolReadObject< MuonFeature_p2 >() );
      MuonFeatureCnv_p2 converter;
      return converter.createTransient( col_vect.get(), mlog );

    }
    else if( compareClassGuid(p0_guid) ) {
      // old version from before TP separation, just return it
      return this->poolReadObject<MuonFeature>();
    }  
    else {
      throw std::runtime_error("Unsupported persistent version");
    }
   
}
