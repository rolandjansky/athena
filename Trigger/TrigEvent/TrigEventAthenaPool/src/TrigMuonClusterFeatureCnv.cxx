/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonClusterFeatureCnv.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_tlp1.h" 

//-----------------------------------------------------------------------------
// Create persistent 
//-----------------------------------------------------------------------------
  
TrigMuonClusterFeature_PERS *
TrigMuonClusterFeatureCnv::createPersistent( TrigMuonClusterFeature *transObj)
{
  MsgStream mlog(msgSvc(), "TrigMuonClusterFeatureConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonClusterFeatureCnv::createPersistent" << endmsg;

  TrigMuonClusterFeature_PERS *persObj = m_converter.createPersistent( transObj, mlog );
  
  return persObj;
}
 
//-----------------------------------------------------------------------------
// Create transient 
//-----------------------------------------------------------------------------
TrigMuonClusterFeature *TrigMuonClusterFeatureCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigMuonClusterFeatureConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonClusterFeatureCnv::createTransient " << endmsg;
  
  static const pool::Guid p1_guid("AE4D5D57-689D-40CB-83B3-CB047884952F");
  static const pool::Guid p0_guid("A7B1865B-55D0-49D2-9778-5E0797FB06FE");

  if( compareClassGuid( p1_guid ) ) {
    std::unique_ptr< TrigMuonClusterFeature_tlp1 > col_vect( poolReadObject< TrigMuonClusterFeature_tlp1 >() );
    return m_converter.createTransient( col_vect.get(), mlog );
  }
  else if( compareClassGuid(p0_guid) ) {
    // old version from before TP separation, just return it
    return poolReadObject<TrigMuonClusterFeature>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version");
  }
}
