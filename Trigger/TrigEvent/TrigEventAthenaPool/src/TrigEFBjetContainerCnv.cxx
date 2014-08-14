/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEFBjetContainerCnv.h"
#include "TrigParticleTPCnv/TrigEFBjetContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigEFBjetContainerCnv_tlp2.h"

struct TrigEFBjetContainerCnv_impl
{
  TrigEFBjetContainerCnv_impl (IMessageSvc* ms)
    : m_log (ms, "TrigEFBjetContainerCnv")
  {}
  MsgStream m_log;
  TrigEFBjetContainerCnv_tlp1   m_TPConverter_tlp1;
  TrigEFBjetContainerCnv_tlp2   m_TPConverter_tlp2;
};

TrigEFBjetContainerCnv::TrigEFBjetContainerCnv( ISvcLocator *svcloc ):
  TrigEFBjetContainerCnvBase(svcloc),
  m_impl (new TrigEFBjetContainerCnv_impl (msgSvc()))
{}

TrigEFBjetContainerCnv::~TrigEFBjetContainerCnv()
{
  delete m_impl;
}

//* createPersistent *//
TrigEFBjetContainer_PERS * TrigEFBjetContainerCnv::createPersistent( TrigEFBjetContainer *transObj) {

  MsgStream mlog(messageService(), "TrigEFBjetContainerConverter" );

  mlog << MSG::DEBUG << "TrigEFBjetContainerCnv::createPersistent called" << endreq;

  TrigEFBjetContainer_PERS * p_EFBjetCont = m_impl->m_TPConverter_tlp2.createPersistent( transObj, m_impl->m_log );
 
  return p_EFBjetCont;
 
}
 
//* createTransient *//
TrigEFBjetContainer * TrigEFBjetContainerCnv::createTransient() {

  MsgStream mlog(messageService(), "TrigEFBjetContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigEFBjetContainerCnv::createTransient called" << endreq;

  static pool::Guid tlp2_guid( "91EA22B6-9273-4206-AEB5-FE23A269C5AA" );
  static pool::Guid tlp1_guid( "F933A067-5788-4AD6-B696-C8B0E988EF2D" );
  static pool::Guid p0_guid( "EDAFEE8F-968E-44A4-BEED-7E7F5C86F397" );
  static pool::Guid p0_guid2( "D08290EE-4CB3-4570-BF7E-0FA008668A24" );
 
  TrigEFBjetContainer *p_collection = 0;

  if ( compareClassGuid( tlp2_guid ) ) {
    
    poolReadObject< TrigEFBjetContainer_PERS >(m_impl->m_TPConverter_tlp2);
    p_collection = m_impl->m_TPConverter_tlp2.createTransient( m_impl->m_log );

  } else if( compareClassGuid( tlp1_guid ) ) {

    poolReadObject< TrigEFBjetContainer_PERS >(m_impl->m_TPConverter_tlp1);
    p_collection = m_impl->m_TPConverter_tlp1.createTransient( m_impl->m_log );
   
  } else if( compareClassGuid( p0_guid ) || compareClassGuid( p0_guid2 ) ) {

    p_collection = poolReadObject< TrigEFBjetContainer >();
    
  } else  throw std::runtime_error( "Unsupported persistent version of TrigEFBjetContainer" );
     
  return p_collection;
   
}
 
