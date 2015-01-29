/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p6.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p6.h"


void TrigMuonEFCbTrackCnv_p6::persToTrans(const TrigMuonEFCbTrack_p6* persObj, TrigMuonEFCbTrack* transObj, MsgStream &log){
    // std::cout << "TrigMuonEFCbTrackCnv_p6::persToTrans called " <<std::endl;

  transObj->m_matchChi2    = persObj->m_matchChi2;
  transObj->m_nIdSctHits   = persObj->m_nIdSctHits;
  transObj->m_nIdPixelHits = persObj->m_nIdPixelHits;
  transObj->m_nTrtHits     = persObj->m_nTrtHits;

   fillTransFromPStore( &m_TrigMuonEFTrackCnv, persObj->m_TrigMuonEFTrack, transObj, log );
}


void TrigMuonEFCbTrackCnv_p6::transToPers(const TrigMuonEFCbTrack* transObj, TrigMuonEFCbTrack_p6* persObj, MsgStream &log) {
    // std::cout << "TrigMuonEFCbTrackCnv_p6::transToPers called " << std::endl;

  persObj->m_matchChi2    = transObj->m_matchChi2;
  persObj->m_nIdSctHits   = transObj->m_nIdSctHits;
  persObj->m_nIdPixelHits = transObj->m_nIdPixelHits;
  persObj->m_nTrtHits     = transObj->m_nTrtHits;

  // std::cout << "TrigMuonEFCbTrackCnv_p6::transToPers before base TrackCnv" << std::endl;
  persObj->m_TrigMuonEFTrack = baseToPersistent( &m_TrigMuonEFTrackCnv, transObj, log );
  // std::cout << "TrigMuonEFCbTrackCnv_p6::transToPers after base TrackCnv" << std::endl;
}
