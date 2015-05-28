/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigVertexCountsCollectionCnv.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCollectionCnv_tlp1.h" 

TrigVertexCountsCollectionCnv::TrigVertexCountsCollectionCnv( ISvcLocator *svcloc )
  : TrigVertexCountsCollectionCnvBase(svcloc),
    m_TPConverter (new TrigVertexCountsCollectionCnv_tlp1)
{}

TrigVertexCountsCollectionCnv::~TrigVertexCountsCollectionCnv()
{
  delete m_TPConverter;
}


//createPersistent 
TrigVertexCountsCollection_PERS * TrigVertexCountsCollectionCnv::createPersistent( TrigVertexCountsCollection *transObj)
{
  MsgStream mlog(messageService(), "TrigVertexCountsCollectionConverter" );

  mlog << MSG::DEBUG << "TrigVertexCountsCollectionCnv::createPersistent" << endreq;

  TrigVertexCountsCollection_PERS* persObj = m_TPConverter->createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method
 

//createTransient
TrigVertexCountsCollection* TrigVertexCountsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigVertexCountsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigVertexCountsCollectionCnv::createTransient" << endreq;

  static pool::Guid tlp1_guid( "2A1D4A4E-D566-4C71-B051-D5D79A856753" );
  static pool::Guid p1_guid( "232B47D0-65CA-4883-AC51-0D76EAEA3194" );
  static pool::Guid trans_guid( "7A4412AD-C11D-4EFD-AE15-D343D2CB28BC" );


  TrigVertexCountsCollection *p_container = 0;

  if(compareClassGuid(tlp1_guid)) {
    poolReadObject<TrigVertexCountsCollection_PERS>(*m_TPConverter);
    p_container = m_TPConverter->createTransient(mlog);
  }
  else if(compareClassGuid(p1_guid)) {
     std::auto_ptr< TrigVertexCountsCollection_p1 > col_vect( poolReadObject< TrigVertexCountsCollection_p1 >() );
     TrigVertexCountsCollectionCnv_p1 converter;
     p_container = converter.createTransient( col_vect.get(), mlog );
  }
  else if(compareClassGuid(trans_guid)) {
    p_container = poolReadObject<TrigVertexCountsCollection>();
  }
  else  {
    throw std::runtime_error( "Unsupported persistent version of TrigVertexCountsCollection" );
  }
  
  return p_container;
   
}//end of create transient method
 
