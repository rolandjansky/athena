/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CombinedMuonFeatureCnv.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_tlp1.h"

//constructor
CombinedMuonFeatureCnv::CombinedMuonFeatureCnv( ISvcLocator* svcloc ) :
  CombinedMuonFeatureCnvBase( svcloc )
{
  m_TPConverter = new CombinedMuonFeatureCnv_tlp1;
}

//destructor
CombinedMuonFeatureCnv::~CombinedMuonFeatureCnv()
{
  delete m_TPConverter;
}

//-----------------------------------------------------------------------------
// Create persistent 
//-----------------------------------------------------------------------------
  
CombinedMuonFeature_PERS *
CombinedMuonFeatureCnv::createPersistent( CombinedMuonFeature *transObj)
{
  MsgStream mlog(messageService(), "CombinedMuonFeatureConverter" );
  
  mlog << MSG::DEBUG << "CombinedMuonFeatureCnv::createPersistent" << endreq;

  CombinedMuonFeature_PERS *persObj = m_TPConverter->createPersistent( transObj, mlog );
  
  return persObj;
}
 
//-----------------------------------------------------------------------------
// Create transient 
//-----------------------------------------------------------------------------
CombinedMuonFeature *CombinedMuonFeatureCnv::createTransient()
{
  MsgStream mlog(messageService(), "CombinedMuonFeatureConverter" );
  
  mlog << MSG::DEBUG << "CombinedMuonFeatureCnv::createTransient " << endreq;
  
  CombinedMuonFeature       *transObj(0);
  
  static pool::Guid p1_guid( "7B8452AC-DDD8-42C5-85BD-D2CE183065A1" );
  static pool::Guid p0_guid( "9DFC54CA-4799-4BCB-A95B-919E7E761112" );
  
   
  if( compareClassGuid( p1_guid ) ) {
    std::auto_ptr< CombinedMuonFeature_tlp1 >   ptr_tlp1( this->poolReadObject< CombinedMuonFeature_tlp1 >() );
    transObj = m_TPConverter->createTransient( ptr_tlp1.get(), mlog );
  }
  else if( compareClassGuid(p0_guid) ) {
    // old version from before TP separation, just return it
    transObj = this->poolReadObject<CombinedMuonFeature>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version");
  }
  return transObj;
}
