/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFIsolationContainerCnv.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainerCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainerCnv_p2.h"

static TrigMuonEFIsolationContainerCnv_p1 TPconverter_p1;
static TrigMuonEFIsolationContainerCnv_p2 TPconverter_p2;


TrigMuonEFIsolationContainerCnv::TrigMuonEFIsolationContainerCnv(ISvcLocator* svcloc) :
  TrigMuonEFIsolationContainerCnvBase(svcloc)
{

}



/// create persistent
TrigMuonEFIsolationContainer_PERS* TrigMuonEFIsolationContainerCnv::createPersistent(TrigMuonEFIsolationContainer* transCont) {
  MsgStream mlog(messageService(), "TrigMuonEFIsolationContainerConverter" );
  mlog << MSG::DEBUG << "TrigMuonEFIsolationContainerCnv::createPersistent" << endreq;
  
  TrigMuonEFIsolationContainer_PERS *persObj = TPconverter_p2.createPersistent(transCont, mlog);

  return persObj;
}

/// create transient
TrigMuonEFIsolationContainer* TrigMuonEFIsolationContainerCnv::createTransient() {

  MsgStream mlog(messageService(), "TrigMuonEFIsolationContainerConverter" );
  mlog << MSG::DEBUG << "TrigMuonEFIsolationContainerCnv::createTransient" << endreq;

  static pool::Guid p1_guid("5C3ECE20-F26F-4811-BA76-B2DC567858BC");
  static pool::Guid p2_guid("B24570F4-BB65-4D5C-A8E2-C44E36E7B0B5");

  TrigMuonEFIsolationContainer* trans_cont(0);

  if ( compareClassGuid(p2_guid) ) {
    std::auto_ptr< TrigMuonEFIsolationContainer_p2 > col_vect( this->poolReadObject< TrigMuonEFIsolationContainer_p2 >() );
    trans_cont = TPconverter_p2.createTransient( col_vect.get(), mlog );
  } else if ( compareClassGuid(p1_guid) ) {
    std::auto_ptr< TrigMuonEFIsolationContainer_p1 > col_vect( this->poolReadObject< TrigMuonEFIsolationContainer_p1 >() );
    trans_cont = TPconverter_p1.createTransient( col_vect.get(), mlog );
  } else {
      
    throw std::runtime_error("Unsupported persistent version of Data container");
    
  }
  return trans_cont;

}
