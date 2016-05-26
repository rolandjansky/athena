/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEFBphysContainerCnv.h"
#include "TrigParticleTPCnv/TrigEFBphysContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigEFBphysContainerCnv_tlp2.h"

struct TrigEFBphysContainerCnv_impl
{
  TrigEFBphysContainerCnv_impl (IMessageSvc* ms)
    : m_log (ms, "TrigEFBphysContainerCnv")
  {}
  MsgStream                      m_log;
  TrigEFBphysContainerCnv_tlp2   m_TPConverter;
};
 
TrigEFBphysContainerCnv::TrigEFBphysContainerCnv( ISvcLocator *svcloc ):
  TrigEFBphysContainerCnvBase(svcloc),
  m_impl (new TrigEFBphysContainerCnv_impl (msgSvc()))
{}
  
TrigEFBphysContainerCnv::~TrigEFBphysContainerCnv()
{
  delete m_impl;
}

//createPersistent
TrigEFBphysContainer_PERS * TrigEFBphysContainerCnv::createPersistent( TrigEFBphysContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigEFBphysContainerConverter" );

  mlog << MSG::DEBUG << "TrigEFBphysContainerCnv::createPersistent called" << endreq;

  TrigEFBphysContainer_PERS * p_EFBphysCont = m_impl->m_TPConverter.createPersistent( transObj, m_impl->m_log );
 
  return p_EFBphysCont;
 
}//end of create persistent method
 

//createTransient
TrigEFBphysContainer * TrigEFBphysContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigEFBphysContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigEFBphysContainerCnv::createTransient called" << endreq;
  
  static pool::Guid tlp2_guid( "D93906C2-4F81-44A7-98BE-71302891FE44" );
  static pool::Guid tlp1_guid( "5F5B068F-32DB-4CC9-BA9E-C74C076AAAA1" );
  static pool::Guid p0_guid( "033EAD89-6721-46F2-9D32-2670BA63B901" );
  static pool::Guid p0_guid2( "760BB4E7-61C1-4365-BE20-C8251E808BA4" );
 
  TrigEFBphysContainer *p_collection = 0;
  if( compareClassGuid( tlp2_guid ) ) {

      poolReadObject< TrigEFBphysContainer_PERS >(m_impl->m_TPConverter);
      p_collection = m_impl->m_TPConverter.createTransient( m_impl->m_log );
   
  } else if( compareClassGuid( tlp1_guid ) ) {

    TrigEFBphysContainerCnv_tlp1  tlp1_Converter;
    poolReadObject< TrigEFBphysContainer_tlp1 >(tlp1_Converter);
    p_collection = tlp1_Converter.createTransient( m_impl->m_log );
   
  }else if( compareClassGuid( p0_guid ) || compareClassGuid( p0_guid2 ) ){

      p_collection = poolReadObject< TrigEFBphysContainer >();
    
    }else  throw std::runtime_error( "Unsupported persistent version of TrigEFBphysContainer" );
     
  return p_collection;
   
}//end of create transient method
 
