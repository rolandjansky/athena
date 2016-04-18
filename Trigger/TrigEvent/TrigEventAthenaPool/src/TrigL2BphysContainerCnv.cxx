/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2BphysContainerCnv.h"
#include "TrigParticleTPCnv/TrigL2BphysContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigL2BphysContainerCnv_tlp2.h"

class TrigL2BphysContainerCnv_impl
{
public:
  TrigL2BphysContainerCnv_impl (IMessageSvc* ms)
    : m_log (ms, "TrigL2BphysContainerCnv")
  {}
  MsgStream                      m_log;
  TrigL2BphysContainerCnv_tlp2   m_TPConverter;
};


TrigL2BphysContainerCnv::TrigL2BphysContainerCnv( ISvcLocator *svcloc ):
  TrigL2BphysContainerCnvBase(svcloc),
  m_impl (new TrigL2BphysContainerCnv_impl (msgSvc()))
{
}

TrigL2BphysContainerCnv::~TrigL2BphysContainerCnv()
{
  delete m_impl;
}


//createPersistent 
TrigL2BphysContainer_PERS * TrigL2BphysContainerCnv::createPersistent( TrigL2BphysContainer *transObj)
{
  m_impl->m_log << MSG::DEBUG
                << "TrigL2BphysContainerCnv::createPersistent called"
                << endreq;

  TrigL2BphysContainer_PERS * p_L2BphysCont =
    m_impl->m_TPConverter.createPersistent( transObj, m_impl->m_log );
 
  return p_L2BphysCont;
 
}//end of create persistent method
 

//createTransient
TrigL2BphysContainer * TrigL2BphysContainerCnv::createTransient()
{
  m_impl->m_log << MSG::DEBUG
                << "TrigL2BphysContainerCnv::createTransient called" << endreq;

  static pool::Guid tlp2_guid( "2E35788D-BE17-4FE1-9DB5-709E05880CEC" );
  static pool::Guid tlp1_guid( "E8AA6B0A-B057-4058-B5A9-AC0B719626AF" );
  static pool::Guid p0_guid( "23296C53-6811-4D19-BD32-816B5DD0508F" );
  static pool::Guid p0_guid2( "390F41C1-CCFA-4A1D-B046-F9F462FD64D0" );
 
  TrigL2BphysContainer *p_collection = 0;
  if( compareClassGuid( tlp2_guid ) ) {
    
    poolReadObject< TrigL2BphysContainer_PERS >(m_impl->m_TPConverter);
    p_collection = m_impl->m_TPConverter.createTransient( m_impl->m_log );
    m_impl->m_log << MSG::DEBUG<< "TrigL2BphysContainerCnv_tlp2" << endreq;
  } else if( compareClassGuid( tlp1_guid ) ) {
    TrigL2BphysContainerCnv_tlp1  tlp1_Converter;
    poolReadObject< TrigL2BphysContainer_tlp1 >(tlp1_Converter);
    p_collection = tlp1_Converter.createTransient( m_impl->m_log );
    
  } else if( compareClassGuid( p0_guid ) || compareClassGuid( p0_guid2 ) ){
	
    p_collection = poolReadObject< TrigL2BphysContainer >();
    
  }else  throw std::runtime_error( "Unsupported persistent version of TrigL2BphysContainer" );
     
  return p_collection;
   
}//end of create transient method
 
