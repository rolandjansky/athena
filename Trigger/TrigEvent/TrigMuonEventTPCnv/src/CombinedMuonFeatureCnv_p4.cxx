/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p4.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p4.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void CombinedMuonFeatureCnv_p4::persToTrans( const CombinedMuonFeature_p4 *persObj,
					     CombinedMuonFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "CombinedMuonFeatureCnv_p4::persToTrans called " << endreq;

  ElementLink< MuonFeatureContainer> muFastTrack;  
  m_elementLinkMuFeatCnv.persToTrans( &persObj->m_muFastTrack, &muFastTrack, log );

  ElementLink< TrigInDetTrackCollection> IDTrack;
  m_elementLinkInDetTrackCnv.persToTrans( &persObj->m_IDTrack, &IDTrack, log );

  *transObj = CombinedMuonFeature (persObj->m_pt,
                                   persObj->m_sigma_pt,
                                   persObj->m_charge,
                                   persObj->m_flag,
                                   muFastTrack,
                                   IDTrack);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void CombinedMuonFeatureCnv_p4::transToPers( const CombinedMuonFeature    *transObj,
					     CombinedMuonFeature_p4 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "CombinedMuonFeatureCnv_p4::transToPers called " << endreq;

  persObj->m_pt           = transObj->pt();
  persObj->m_sigma_pt     = transObj->sigma_pt_raw();
  persObj->m_charge       = transObj->charge();
  persObj->m_flag         = transObj->getFlag(),

  m_elementLinkMuFeatCnv.transToPers( &transObj->muFastTrackLink(), &persObj->m_muFastTrack, log );
  m_elementLinkInDetTrackCnv.transToPers( &transObj->IDTrackLink(), &persObj->m_IDTrack, log );
   
}
