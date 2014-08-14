/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSpacePointCountsCnv.h"
#include "RootConversions/TConverterRegistry.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1_old_cnv.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p3.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p3.h"

TrigSpacePointCountsCnv::TrigSpacePointCountsCnv(ISvcLocator *svcloc): TrigSpacePointCountsCnvBase(svcloc) {}
TrigSpacePointCountsCnv::~TrigSpacePointCountsCnv(){}


TrigSpacePointCounts_PERS* TrigSpacePointCountsCnv::createPersistent(TrigSpacePointCounts* transObj) {
  MsgStream mlog(messageService(), "TrigSpacePointCountsConverter" );

  mlog << MSG::DEBUG << "TrigSpacePointCountsCnv::createPersistent" << endreq;

  TrigSpacePointCountsCnv_p3 converter;
   
  TrigSpacePointCounts_PERS *persObj = converter.createPersistent( transObj, mlog );

  return persObj;
}


TrigSpacePointCounts* TrigSpacePointCountsCnv::createTransient() {
  MsgStream mlog(messageService(), "TrigSpacePointCountsConverter" );

  mlog << MSG::DEBUG << "TrigSpacePointCountsCnv::createTransient " << endreq;

  static pool::Guid p3_guid("43E61EDF-2902-4F64-8A89-F625DB7DA7CC");
  static pool::Guid p2_guid("3A6CFBCF-E2AE-4E3D-A965-091718A5CB9F");
  static pool::Guid p1_guid("8BF48F79-C6C2-4AA7-8180-16BC6C39280F");
  static pool::Guid p0_guid("1BCAD9FD-DAFE-4E50-9A37-C75E822E6D02");
 
  if(compareClassGuid(p3_guid)) {
    std::auto_ptr<TrigSpacePointCounts_p3 > pers_ptr( poolReadObject< TrigSpacePointCounts_p3 >() );
    TrigSpacePointCountsCnv_p3 converter;
    return converter.createTransient( pers_ptr.get(), mlog );
  } 
  if(compareClassGuid(p2_guid)) {
    std::auto_ptr<TrigSpacePointCounts_p2 > pers_ptr( poolReadObject< TrigSpacePointCounts_p2 >() );
    TrigSpacePointCountsCnv_p2 converter;
    return converter.createTransient( pers_ptr.get(), mlog );
  } 
  if(compareClassGuid(p1_guid)) {
    std::auto_ptr<TrigSpacePointCounts_p1 > pers_ptr( poolReadObject< TrigSpacePointCounts_p1 >() );
    TrigSpacePointCountsCnv_p1 converter;
    return converter.createTransient( pers_ptr.get(), mlog );
  }
  else if( compareClassGuid(p0_guid) ) {
    // old version from before TP separation, just return it
    return this->poolReadObject<TrigSpacePointCounts>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version");
  }

}


StatusCode TrigSpacePointCountsCnv::initialize()
{
  static bool did_rootcnv = false;
  if (!did_rootcnv) {
    did_rootcnv = true;
    static TrigSpacePointCounts_p1_old_cnv cnv;
    TConverterRegistry::Instance()->AddConverter (&cnv);
  }
  return TrigSpacePointCountsCnvBase::initialize();
}

