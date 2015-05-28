/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonClusterFeatureContainerCnv.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainerCnv_tlp1.h"

TrigMuonClusterFeatureContainerCnv::TrigMuonClusterFeatureContainerCnv( ISvcLocator *svcloc ):
  TrigMuonClusterFeatureContainerCnvBase(svcloc),
  m_msgSvc( msgSvc() ),
  m_log( m_msgSvc, "TrigMuonClusterFeatureContainerCnv" ),
  m_TPConverter (new TrigMuonClusterFeatureContainerCnv_tlp1)
{}
  
TrigMuonClusterFeatureContainerCnv::~TrigMuonClusterFeatureContainerCnv()
{
  delete m_TPConverter;
}
  

//createPersistent 
TrigMuonClusterFeatureContainer_PERS * TrigMuonClusterFeatureContainerCnv::createPersistent( TrigMuonClusterFeatureContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigMuonClusterFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonClusterFeatureContainerCnv::createPersistent called" << endreq;
  
  TrigMuonClusterFeatureContainer_PERS * p_cont = m_TPConverter->createPersistent( transObj, m_log );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigMuonClusterFeatureContainer * TrigMuonClusterFeatureContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigMuonClusterFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonClusterFeatureContainerCnv::createTransient called" << endreq;
  
  static pool::Guid tlp1_guid( "036D8CC1-5FBD-4676-9D84-F233567F52D2" );
  static pool::Guid p0_guid( "5B09201A-D1CD-43BA-A1B5-786189444579" );
  
  TrigMuonClusterFeatureContainer *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
      poolReadObject< TrigMuonClusterFeatureContainer_PERS >(*m_TPConverter);
      p_collection = m_TPConverter->createTransient( m_log );
      
    }else if( compareClassGuid( p0_guid ) ){
      
      p_collection = poolReadObject< TrigMuonClusterFeatureContainer >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of TrigMuonClusterFeatureContainer" );
  
  return p_collection;
  
}//end of create transient method

