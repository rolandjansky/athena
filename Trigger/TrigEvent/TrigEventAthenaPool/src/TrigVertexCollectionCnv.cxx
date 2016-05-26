/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigVertexCollectionCnv.h"
#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_tlp2.h"

#include "TrigInDetEventTPCnv/TrigVertexCollection_tlp1.h"
#include "TrigInDetEventTPCnv/TrigVertexCollection_tlp2.h"

TrigVertexCollectionCnv::TrigVertexCollectionCnv( ISvcLocator *svcloc ):
  TrigVertexCollectionCnvBase(svcloc),
  m_msgSvc( msgSvc() ),
  m_log( m_msgSvc, "TrigVertexCollectionCnv" ),
  m_TPConverter (new TrigVertexCollectionCnv_tlp2)
{}

TrigVertexCollectionCnv::~TrigVertexCollectionCnv()
{
  delete m_TPConverter;
}

//createPersistent 
TrigVertexCollection_PERS * TrigVertexCollectionCnv::createPersistent( TrigVertexCollection *transObj)
{
  MsgStream mlog(messageService(), "TrigVertexCollectionConverter" );

  mlog << MSG::DEBUG << "TrigVertexCollectionCnv::createPersistent called" << endreq;

  TrigVertexCollection_PERS * p_VtxColl = m_TPConverter->createPersistent( transObj, m_log );

  return p_VtxColl;
 
}//end of create persistent method
 

//createTransient
TrigVertexCollection * TrigVertexCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigVertexCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigVertexCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp2_guid( "438C9232-0F3E-4A8B-A16E-6F6275388DE4" );
  static pool::Guid tlp1_guid( "10E18C4E-9BCA-4F25-993C-EBDF0642C119" );
  static pool::Guid p0_guid( "0974DF97-5B51-4416-8FBE-42BAE0C54010" );
  static pool::Guid p0_guid2( "E2C600D6-CD4B-4B7B-9C09-93CE9FF435A1" );
 
  TrigVertexCollection *p_collection = 0;
  if( compareClassGuid( tlp2_guid ) )     
    {
      mlog << MSG::DEBUG << "TrigVertexCollectionCnv::createPersistent tlp2 called" << endreq;
      poolReadObject< TrigVertexCollection_PERS >(*m_TPConverter);
      p_collection = m_TPConverter->createTransient( m_log );
   
    } else if( compareClassGuid( tlp1_guid ) ) {

      mlog << MSG::DEBUG << "TrigVertexCollectionCnv::createPersistent tlp1 called" << endreq;
      TrigVertexCollectionCnv_tlp1  tlp1_Converter;
      poolReadObject< TrigVertexCollection_tlp1 >(tlp1_Converter);
      p_collection = tlp1_Converter.createTransient( m_log );

    } else if( compareClassGuid( p0_guid ) || compareClassGuid( p0_guid2 ) ){
      mlog << MSG::DEBUG << "TrigVertexCollectionCnv::createPersistent p0 called" << endreq;
      p_collection = poolReadObject< TrigVertexCollection >();
    
    }else  throw std::runtime_error( "Unsupported persistent version of TrigVertexCollection" );
     
  return p_collection;
   
}//end of create transient method
 
