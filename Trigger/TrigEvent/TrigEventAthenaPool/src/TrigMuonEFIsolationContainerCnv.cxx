/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFIsolationContainerCnv.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationContainer_p2.h"


TrigMuonEFIsolationContainerCnv::TrigMuonEFIsolationContainerCnv(ISvcLocator* svcloc) :
  TrigMuonEFIsolationContainerCnvBase(svcloc)
{

}



/// create persistent
TrigMuonEFIsolationContainer_PERS* TrigMuonEFIsolationContainerCnv::createPersistent(TrigMuonEFIsolationContainer* transCont) {
  MsgStream mlog(msgSvc(), "TrigMuonEFIsolationContainerConverter" );
  mlog << MSG::DEBUG << "TrigMuonEFIsolationContainerCnv::createPersistent" << endmsg;
  
  TrigMuonEFIsolationContainer_PERS *persObj = m_converter_p2.createPersistent(transCont, mlog);

  return persObj;
}

/// create transient
TrigMuonEFIsolationContainer* TrigMuonEFIsolationContainerCnv::createTransient() {

  MsgStream mlog(msgSvc(), "TrigMuonEFIsolationContainerConverter" );
  mlog << MSG::DEBUG << "TrigMuonEFIsolationContainerCnv::createTransient" << endmsg;

  static const pool::Guid p1_guid("5C3ECE20-F26F-4811-BA76-B2DC567858BC");
  static const pool::Guid p2_guid("B24570F4-BB65-4D5C-A8E2-C44E36E7B0B5");

  TrigMuonEFIsolationContainer* trans_cont(0);

  if ( compareClassGuid(p2_guid) ) {
    std::unique_ptr< TrigMuonEFIsolationContainer_p2 > col_vect( this->poolReadObject< TrigMuonEFIsolationContainer_p2 >() );
    trans_cont = m_converter_p2.createTransient( col_vect.get(), mlog );
  } else if ( compareClassGuid(p1_guid) ) {
    std::unique_ptr< TrigMuonEFIsolationContainer_p1 > col_vect( this->poolReadObject< TrigMuonEFIsolationContainer_p1 >() );
    trans_cont = m_converter_p1.createTransient( col_vect.get(), mlog );
  } else {
      
    throw std::runtime_error("Unsupported persistent version of Data container");
    
  }
  return trans_cont;

}
