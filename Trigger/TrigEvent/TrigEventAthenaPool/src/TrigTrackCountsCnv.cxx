/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTrackCountsCnv.h"
#include "TrigInDetEventTPCnv/TrigTrackCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigTrackCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCnv_p2.h"

TrigTrackCountsCnv::TrigTrackCountsCnv(ISvcLocator *svcloc): TrigTrackCountsCnvBase(svcloc) {}
TrigTrackCountsCnv::~TrigTrackCountsCnv(){}


TrigTrackCounts_PERS* TrigTrackCountsCnv::createPersistent(TrigTrackCounts* transObj) {
  MsgStream mlog(messageService(), "TrigTrackCountsConverter" );

  mlog << MSG::DEBUG << "TrigTrackCountsCnv::createPersistent" << endreq;

  TrigTrackCountsCnv_p2   converter;
   
  TrigTrackCounts_PERS *persObj = converter.createPersistent( transObj, mlog );

  return persObj;
}


TrigTrackCounts* TrigTrackCountsCnv::createTransient() {
  MsgStream mlog(messageService(), "TrigTrackCountsConverter" );

  mlog << MSG::DEBUG << "TrigTrackCountsCnv::createTransient " << endreq;

  static pool::Guid p2_guid("BD8BB599-AE38-45CC-93DB-27F67C23DB62");
  static pool::Guid p1_guid("2DCA396C-5CC1-4B6A-8B76-E3EBA4F81A61");
  static pool::Guid p0_guid("6277C97C-0EAA-4922-892E-E88C1FA01BA0");
  
  if(compareClassGuid(p2_guid)) {
    std::auto_ptr< TrigTrackCounts_p2 > pers_ptr( poolReadObject< TrigTrackCounts_p2 >() );
    TrigTrackCountsCnv_p2 converter;
    return converter.createTransient( pers_ptr.get(), mlog );
  }
  else if(compareClassGuid(p1_guid)) {
    std::auto_ptr< TrigTrackCounts_p1 > pers_ptr( poolReadObject< TrigTrackCounts_p1 >() );
    TrigTrackCountsCnv_p1 converter;
    return converter.createTransient( pers_ptr.get(), mlog );
  }
  else if(compareClassGuid(p0_guid)) {
    // old version from before TP separation, just return it
    return this->poolReadObject<TrigTrackCounts>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version");
  }

}
