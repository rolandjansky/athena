/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFInfoContainerCnv.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_tlp2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_tlp2.h"

TrigMuonEFInfoContainerCnv::TrigMuonEFInfoContainerCnv( ISvcLocator *svcloc ):
  TrigMuonEFInfoContainerCnvBase(svcloc),
  m_msgSvc( msgSvc() ),
  m_log( m_msgSvc, "TrigMuonEFInfoContainerCnv" ),
  m_TPConverter1 (new TrigMuonEFInfoContainerCnv_tlp1),
  m_TPConverter2 (new TrigMuonEFInfoContainerCnv_tlp2)
{}
  
TrigMuonEFInfoContainerCnv::~TrigMuonEFInfoContainerCnv(){
    delete m_TPConverter1;
    delete m_TPConverter2;
}

//createPersistent 
TrigMuonEFInfoContainer_PERS * TrigMuonEFInfoContainerCnv::createPersistent( TrigMuonEFInfoContainer *transObj) {
  MsgStream mlog(messageService(), "TrigMuonEFInfoContainerConverter" );
  mlog << MSG::DEBUG << "TrigMuonEFInfoContainerCnv::createPersistent called" << endreq;
  
  TrigMuonEFInfoContainer_PERS * p_muEFcont = m_TPConverter2->createPersistent( transObj, m_log );
  
  return p_muEFcont; 
}


TrigMuonEFInfoContainer * TrigMuonEFInfoContainerCnv::createTransient(){

  MsgStream mlog(messageService(), "TrigMuonEFInfoContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonEFInfoContainerCnv::createTransient called" << endreq;
  
  static pool::Guid p1_guid( "B768E99D-9CC3-4368-ADD5-36816D2A0298" );
  static pool::Guid p2_guid( "AF8CBEC0-4BBF-438A-82DE-873F133F4821" );
  
  TrigMuonEFInfoContainer *p_collection = 0;
  
  if( compareClassGuid( p2_guid ) ) {
    poolReadObject< TrigMuonEFInfoContainer_PERS >(*m_TPConverter2);
    p_collection = m_TPConverter2->createTransient( m_log );
  }
  else if( compareClassGuid( p1_guid ) ) {
    poolReadObject< TrigMuonEFInfoContainer_tlp1 >(*m_TPConverter1);
    p_collection = m_TPConverter1->createTransient( m_log );
  }
  else  throw std::runtime_error( "Unsupported persistent version of TrigMuonEFInfoContainer" );

  return p_collection;  
}
