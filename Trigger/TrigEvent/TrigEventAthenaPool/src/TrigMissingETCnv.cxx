/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMissingETCnv.h"
#include "TrigMissingEtEventTPCnv/TrigMissingET_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingET_p2.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p2.h"

TrigMissingETCnv::TrigMissingETCnv(ISvcLocator *svcloc): TrigMissingETCnvBase(svcloc) {}
TrigMissingETCnv::~TrigMissingETCnv(){}



TrigMissingET_PERS* TrigMissingETCnv::createPersistent(TrigMissingET* transObj) {
  MsgStream mlog(messageService(), "TrigMissingETConverter" );

  mlog << MSG::DEBUG << "TrigMissingETCnv::createPersistent" << endreq;

//   TrigMissingETCnv_p1   converter;
  TrigMissingETCnv_p2   converter;
   
  TrigMissingET_PERS *persObj = converter.createPersistent( transObj, mlog );

  return persObj;
}


TrigMissingET* TrigMissingETCnv::createTransient() {
  MsgStream mlog(messageService(), "TrigMissingETConverter" );

  mlog << MSG::DEBUG << "TrigMissingETCnv::createTransient " << endreq;

  static pool::Guid p2_guid("9972F10B-2B6A-42A7-AF71-D8FB60F07C6D");
  static pool::Guid p1_guid("A65F0499-A789-4D66-9930-E8651FE91F8A");
  static pool::Guid p0_guid("1403772C-7444-4340-930B-ABD51C243A09");
  
  if( compareClassGuid(p2_guid) ) {
    
    std::auto_ptr< TrigMissingET_p2 > col_vect( poolReadObject< TrigMissingET_p2 >() );
    TrigMissingETCnv_p2 converter;
    return converter.createTransient( col_vect.get(), mlog );

  }
  else if( compareClassGuid(p1_guid) ) {
    
    std::auto_ptr< TrigMissingET_p1 > col_vect( poolReadObject< TrigMissingET_p1 >() );
    TrigMissingETCnv_p1 converter;
    return converter.createTransient( col_vect.get(), mlog );

  }
  else if( compareClassGuid(p0_guid) ) {
    // old version from before TP separation, just return it
    return this->poolReadObject<TrigMissingET>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version");
  }

}
