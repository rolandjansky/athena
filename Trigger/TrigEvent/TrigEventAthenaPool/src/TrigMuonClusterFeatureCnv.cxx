/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonClusterFeatureCnv.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_tlp1.h" 
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_tlp1.h"

static TrigMuonClusterFeatureCnv_tlp1 TPConverter;

//-----------------------------------------------------------------------------
// Create persistent 
//-----------------------------------------------------------------------------
  
TrigMuonClusterFeature_PERS *
TrigMuonClusterFeatureCnv::createPersistent( TrigMuonClusterFeature *transObj)
{
  MsgStream mlog(messageService(), "TrigMuonClusterFeatureConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonClusterFeatureCnv::createPersistent" << endreq;

  TrigMuonClusterFeature_PERS *persObj = TPConverter.createPersistent( transObj, mlog );
  
  return persObj;
}
 
//-----------------------------------------------------------------------------
// Create transient 
//-----------------------------------------------------------------------------
TrigMuonClusterFeature *TrigMuonClusterFeatureCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigMuonClusterFeatureConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonClusterFeatureCnv::createTransient " << endreq;
  
  static pool::Guid p1_guid("AE4D5D57-689D-40CB-83B3-CB047884952F");
  static pool::Guid p0_guid("A7B1865B-55D0-49D2-9778-5E0797FB06FE");

  if( compareClassGuid( p1_guid ) ) {
    std::auto_ptr< TrigMuonClusterFeature_tlp1 > col_vect( poolReadObject< TrigMuonClusterFeature_tlp1 >() );
    return TPConverter.createTransient( col_vect.get(), mlog );
  }
  else if( compareClassGuid(p0_guid) ) {
    // old version from before TP separation, just return it
    return poolReadObject<TrigMuonClusterFeature>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version");
  }
}
