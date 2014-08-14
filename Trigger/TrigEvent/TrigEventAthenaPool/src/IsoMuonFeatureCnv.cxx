/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IsoMuonFeatureCnv.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_tlp1.h"

//constructor
IsoMuonFeatureCnv::IsoMuonFeatureCnv( ISvcLocator* svcloc ) :
  IsoMuonFeatureCnvBase( svcloc ),
  m_TPConverter (new IsoMuonFeatureCnv_tlp1)
{}

//destructor
IsoMuonFeatureCnv::~IsoMuonFeatureCnv()
{
  delete m_TPConverter;
}

//-----------------------------------------------------------------------------
// Create persistent 
//-----------------------------------------------------------------------------
  
IsoMuonFeature_PERS *
IsoMuonFeatureCnv::createPersistent( IsoMuonFeature *transObj)
{
  MsgStream mlog(messageService(), "IsoMuonFeatureConverter" );
  
  mlog << MSG::DEBUG << "IsoMuonFeatureCnv::createPersistent" << endreq;

  IsoMuonFeature_PERS *persObj = m_TPConverter->createPersistent( transObj, mlog );
  
  return persObj;
}
 
//-----------------------------------------------------------------------------
// Create transient 
//-----------------------------------------------------------------------------
IsoMuonFeature *IsoMuonFeatureCnv::createTransient()
{
  MsgStream mlog(messageService(), "IsoMuonFeatureConverter" );
  
  mlog << MSG::DEBUG << "IsoMuonFeatureCnv::createTransient " << endreq;
  
  IsoMuonFeature       *transObj(0);
  
  static pool::Guid p1_guid("31A90EEE-AF5A-4CAF-9A83-F523D141C4CF");
  static pool::Guid p0_guid("60ECF1E1-408A-43CA-9858-62AAFE8041FF");

  if( compareClassGuid( p1_guid ) ) {
    std::auto_ptr< IsoMuonFeature_tlp1 >   ptr_tlp1( this->poolReadObject< IsoMuonFeature_tlp1 >() );
    transObj = m_TPConverter->createTransient( ptr_tlp1.get(), mlog );
  }
  else if( compareClassGuid(p0_guid) ) {
    // old version from before TP separation, just return it
    transObj = this->poolReadObject<IsoMuonFeature>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version");
  }
  return transObj;
}
