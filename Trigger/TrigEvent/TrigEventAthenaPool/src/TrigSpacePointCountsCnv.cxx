/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSpacePointCountsCnv.h"
#include "RootConversions/TConverterRegistry.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1_old_cnv.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p3.h"


TrigSpacePointCountsCnv::TrigSpacePointCountsCnv(ISvcLocator *svcloc): TrigSpacePointCountsCnvBase(svcloc) {}
TrigSpacePointCountsCnv::~TrigSpacePointCountsCnv(){}


TrigSpacePointCounts_PERS* TrigSpacePointCountsCnv::createPersistent(TrigSpacePointCounts* transObj) {
  MsgStream mlog(msgSvc(), "TrigSpacePointCountsConverter" );

  mlog << MSG::DEBUG << "TrigSpacePointCountsCnv::createPersistent" << endmsg;
   
  TrigSpacePointCounts_PERS *persObj = m_converter_p3.createPersistent( transObj, mlog );

  return persObj;
}


TrigSpacePointCounts* TrigSpacePointCountsCnv::createTransient() {
  MsgStream mlog(msgSvc(), "TrigSpacePointCountsConverter" );

  mlog << MSG::DEBUG << "TrigSpacePointCountsCnv::createTransient " << endmsg;

  static const pool::Guid p3_guid("43E61EDF-2902-4F64-8A89-F625DB7DA7CC");
  static const pool::Guid p2_guid("3A6CFBCF-E2AE-4E3D-A965-091718A5CB9F");
  static const pool::Guid p1_guid("8BF48F79-C6C2-4AA7-8180-16BC6C39280F");
  static const pool::Guid p0_guid("1BCAD9FD-DAFE-4E50-9A37-C75E822E6D02");
 
  if(compareClassGuid(p3_guid)) {
    std::unique_ptr<TrigSpacePointCounts_p3 > pers_ptr( poolReadObject< TrigSpacePointCounts_p3 >() );
    return m_converter_p3.createTransient( pers_ptr.get(), mlog );
  } 
  if(compareClassGuid(p2_guid)) {
    std::unique_ptr<TrigSpacePointCounts_p2 > pers_ptr( poolReadObject< TrigSpacePointCounts_p2 >() );
    return m_converter_p2.createTransient( pers_ptr.get(), mlog );
  } 
  if(compareClassGuid(p1_guid)) {
    std::unique_ptr<TrigSpacePointCounts_p1 > pers_ptr( poolReadObject< TrigSpacePointCounts_p1 >() );
    return m_converter_p1.createTransient( pers_ptr.get(), mlog );
  }
  else if( compareClassGuid(p0_guid) ) {
    // old version from before TP separation, just return it
    return this->poolReadObject<TrigSpacePointCounts>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version");
  }

}

namespace {
  // Helper to be able to call thread-unsafe code during initialize as we
  // currently cannot mark Converter::initialize() as thread-unsafe.
  StatusCode loadConverter ATLAS_NOT_THREAD_SAFE() {
    static TrigSpacePointCounts_p1_old_cnv cnv;
    TConverterRegistry::Instance()->AddConverter (&cnv);
    return StatusCode::SUCCESS;
  }
}

StatusCode TrigSpacePointCountsCnv::initialize()
{
  [[maybe_unused]] static const bool did_rootcnv = []{
    StatusCode sc ATLAS_THREAD_SAFE = loadConverter();
    return sc.isSuccess();
  }();

  return TrigSpacePointCountsCnvBase::initialize();
}

