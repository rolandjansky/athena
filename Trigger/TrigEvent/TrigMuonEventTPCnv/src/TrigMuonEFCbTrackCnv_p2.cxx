/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p2.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p2.h"


void TrigMuonEFCbTrackCnv_p2::persToTrans(const TrigMuonEFCbTrack_p2* persObj,
					  TrigMuonEFCbTrack* transObj,
					  MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFCbTrackCnv_p2::persToTrans called " << endreq;

  transObj->m_matchChi2    = persObj->m_matchChi2;
  transObj->m_nIdSctHits   = persObj->m_nIdSctHits;
  transObj->m_nIdPixelHits = persObj->m_nIdPixelHits;
  transObj->m_nTrtHits     = persObj->m_nTrtHits;

   fillTransFromPStore( &m_TrigMuonEFTrackCnv, persObj->m_TrigMuonEFTrack, transObj, log );
}


void TrigMuonEFCbTrackCnv_p2::transToPers(const TrigMuonEFCbTrack* /*transObj*/,
					  TrigMuonEFCbTrack_p2* /*persObj*/,
					  MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFCbTrackCnv_p2::transToPers called " << endreq;
//
//  persObj->m_matchChi2    = transObj->m_matchChi2;
//  persObj->m_nIdSctHits   = transObj->m_nIdSctHits;
//  persObj->m_nIdPixelHits = transObj->m_nIdPixelHits;
//  persObj->m_nTrtHits     = transObj->m_nTrtHits;
//
//  persObj->m_TrigMuonEFTrack = baseToPersistent( &m_TrigMuonEFTrackCnv, transObj, log );
}
