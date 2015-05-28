/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTrackCountsCollectionCnv.h"
#include "TrigInDetEventTPCnv/TrigTrackCountsCollectionCnv_tlp1.h"

TrigTrackCountsCollectionCnv::TrigTrackCountsCollectionCnv( ISvcLocator *svcloc ):
  TrigTrackCountsCollectionCnvBase(svcloc),
  m_msgSvc( msgSvc() ),
  m_log( m_msgSvc, "TrigTrackCountsCollectionCnv" ),
  m_TPConverter (new TrigTrackCountsCollectionCnv_tlp1)
{}
  
TrigTrackCountsCollectionCnv::~TrigTrackCountsCollectionCnv()
{
  delete m_TPConverter;
}

//createPersistent 
TrigTrackCountsCollection_PERS * TrigTrackCountsCollectionCnv::createPersistent( TrigTrackCountsCollection *transObj)
{
  MsgStream mlog(messageService(), "TrigTrackCountsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigTrackCountsCollectionCnv::createPersistent called" << endreq;
  
  TrigTrackCountsCollection_PERS * p_cont = m_TPConverter->createPersistent( transObj, m_log );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigTrackCountsCollection * TrigTrackCountsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigTrackCountsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigTrackCountsCollectionCnv::createTransient called" << endreq;
  
  static pool::Guid tlp1_guid( "FCEB4390-F8FB-4C93-9538-105BCABE487C" );
  static pool::Guid p0_guid( "7A4412AD-C11D-4EFD-AE15-D343D2CB28BC" );
  
  TrigTrackCountsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
      poolReadObject< TrigTrackCountsCollection_PERS >(*m_TPConverter);
      p_collection = m_TPConverter->createTransient( m_log );
      
    }else if( compareClassGuid( p0_guid ) ){
      
      p_collection = poolReadObject< TrigTrackCountsCollection >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of TrigTrackCountsCollection" );
  
  return p_collection;
  
}//end of create transient method

