/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CosmicMuonCollectionCnv.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCollectionCnv_tlp1.h"

CosmicMuonCollectionCnv::CosmicMuonCollectionCnv( ISvcLocator *svcloc )
  : CosmicMuonCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "CosmicMuonCollectionCnv" ),
    m_TPConverter (new CosmicMuonCollectionCnv_tlp1)
    {}
  
CosmicMuonCollectionCnv::~CosmicMuonCollectionCnv()
{
  delete m_TPConverter;
}

//createPersistent 
CosmicMuonCollection_PERS * CosmicMuonCollectionCnv::createPersistent( CosmicMuonCollection *transObj)
{
  MsgStream mlog(messageService(), "CosmicMuonCollectionConverter" );
  
  mlog << MSG::DEBUG << "CosmicMuonCollectionCnv::createPersistent called" << endreq;
  
  CosmicMuonCollection_PERS * p_cont = m_TPConverter->createPersistent( transObj, m_log );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
CosmicMuonCollection * CosmicMuonCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CosmicMuonCollectionConverter" );
  
  mlog << MSG::DEBUG << "CosmicMuonCollectionCnv::createTransient called" << endreq;
  
  static pool::Guid tlp1_guid( "5B1118A0-027C-4B33-983C-8B49099B8021" );
  static pool::Guid p0_guid( "E7993C41-1213-4EC3-8EF2-35F2A5AD4E83" );
  
  CosmicMuonCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
      poolReadObject< CosmicMuonCollection_PERS >(*m_TPConverter);
      p_collection = m_TPConverter->createTransient( m_log );
      
    }else if( compareClassGuid( p0_guid ) ){
      
      p_collection = poolReadObject< CosmicMuonCollection >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of CosmicMuonCollection" );
  
  return p_collection;
  
}//end of create transient method

