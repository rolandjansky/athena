/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p6.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p6.h"


void TrigMuonEFCbTrackCnv_p6::persToTrans(const TrigMuonEFCbTrack_p6* persObj, TrigMuonEFCbTrack* transObj, MsgStream &log){
    // std::cout << "TrigMuonEFCbTrackCnv_p6::persToTrans called " <<std::endl;

  transObj->setMatchChi2    (persObj->m_matchChi2);
  transObj->setNIdSctHits   (persObj->m_nIdSctHits);
  transObj->setNIdPixelHits (persObj->m_nIdPixelHits);
  transObj->setNTrtHits     (persObj->m_nTrtHits);

   fillTransFromPStore( &m_TrigMuonEFTrackCnv, persObj->m_TrigMuonEFTrack, transObj, log );
}


void TrigMuonEFCbTrackCnv_p6::transToPers(const TrigMuonEFCbTrack* transObj, TrigMuonEFCbTrack_p6* persObj, MsgStream &log) {
    // std::cout << "TrigMuonEFCbTrackCnv_p6::transToPers called " << std::endl;

  persObj->m_matchChi2    = transObj->matchChi2();
  persObj->m_nIdSctHits   = transObj->NIdSctHits();
  persObj->m_nIdPixelHits = transObj->NIdPixelHits();
  persObj->m_nTrtHits     = transObj->NTrtHits();

  // std::cout << "TrigMuonEFCbTrackCnv_p6::transToPers before base TrackCnv" << std::endl;
  persObj->m_TrigMuonEFTrack = baseToPersistent( &m_TrigMuonEFTrackCnv, transObj, log );
  // std::cout << "TrigMuonEFCbTrackCnv_p6::transToPers after base TrackCnv" << std::endl;
}
