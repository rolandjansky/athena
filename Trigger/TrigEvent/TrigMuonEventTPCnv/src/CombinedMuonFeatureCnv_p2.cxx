/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p2.h"
#undef private
#undef protected
 
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p2.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void CombinedMuonFeatureCnv_p2::persToTrans( const CombinedMuonFeature_p2 *persObj,
					     CombinedMuonFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "CombinedMuonFeatureCnv_p2::persToTrans called " << endreq;

  transObj->m_pt          = persObj->m_pt;
  transObj->m_sigma_pt    = persObj->m_sigma_pt;
  m_elementLinkMuFeatCnv.persToTrans( &persObj->m_muFastTrack, &transObj->m_muFastTrack, log );
  m_elementLinkInDetTrackCnv.persToTrans( &persObj->m_IDTrack, &transObj->m_IDTrack, log );
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void CombinedMuonFeatureCnv_p2::transToPers( const CombinedMuonFeature    *transObj,
					     CombinedMuonFeature_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "CombinedMuonFeatureCnv_p2::transToPers called " << endreq;

  persObj->m_pt           = transObj->m_pt;
  persObj->m_sigma_pt     = transObj->m_sigma_pt;
  m_elementLinkMuFeatCnv.transToPers( &transObj->m_muFastTrack, &persObj->m_muFastTrack, log );
  m_elementLinkInDetTrackCnv.transToPers( &transObj->m_IDTrack, &persObj->m_IDTrack, log );
   
}
