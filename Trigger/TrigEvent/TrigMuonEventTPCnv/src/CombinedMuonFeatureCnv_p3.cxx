/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p3.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p3.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void CombinedMuonFeatureCnv_p3::persToTrans( const CombinedMuonFeature_p3 *persObj,
					     CombinedMuonFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "CombinedMuonFeatureCnv_p3::persToTrans called " << endmsg;

  ElementLink< MuonFeatureContainer> muFastTrack;  
  m_elementLinkMuFeatCnv.persToTrans( &persObj->m_muFastTrack, &muFastTrack, log );

  ElementLink< TrigInDetTrackCollection> IDTrack;
  m_elementLinkInDetTrackCnv.persToTrans( &persObj->m_IDTrack, &IDTrack, log );

  *transObj = CombinedMuonFeature (persObj->m_pt,
                                   persObj->m_sigma_pt,
                                   0, // m_charge
                                   0, // m_flag
                                   muFastTrack,
                                   IDTrack);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void CombinedMuonFeatureCnv_p3::transToPers( const CombinedMuonFeature    *transObj,
					     CombinedMuonFeature_p3 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "CombinedMuonFeatureCnv_p3::transToPers called " << endmsg;

  persObj->m_pt           = transObj->pt();
  persObj->m_sigma_pt     = transObj->sigma_pt_raw();

  m_elementLinkMuFeatCnv.transToPers( &transObj->muFastTrackLink(), &persObj->m_muFastTrack, log );
  m_elementLinkInDetTrackCnv.transToPers( &transObj->IDTrackLink(), &persObj->m_IDTrack, log );
}
