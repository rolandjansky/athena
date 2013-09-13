/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p7.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p7.h"

typedef ElementLinkCnv_p3<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;

/// pre-allocate the track converter
static TrackLinkCnv_t         trackCnv;

void TrigMuonEFCbTrackCnv_p7::persToTrans(const TrigMuonEFCbTrack_p7* persObj, TrigMuonEFCbTrack* transObj, MsgStream &log){
    // std::cout << "TrigMuonEFCbTrackCnv_p7::persToTrans called " <<std::endl;

  //log << MSG::DEBUG << "Called TrigMuonEFCbTrackCnv_p7::persToTrans" << endreq;
  transObj->m_matchChi2    = persObj->m_matchChi2;
  transObj->m_nIdSctHits   = persObj->m_nIdSctHits;
  transObj->m_nIdPixelHits = persObj->m_nIdPixelHits;
  transObj->m_nTrtHits     = persObj->m_nTrtHits;

  trackCnv.persToTrans(&persObj->m_idTrackLink, &transObj->m_idTrackLink, log);

   fillTransFromPStore( &m_TrigMuonEFTrackCnv, persObj->m_TrigMuonEFTrack, transObj, log );
}


void TrigMuonEFCbTrackCnv_p7::transToPers(const TrigMuonEFCbTrack* transObj, TrigMuonEFCbTrack_p7* persObj, MsgStream &log) {
    // std::cout << "TrigMuonEFCbTrackCnv_p7::transToPers called " << std::endl;

  //log << MSG::DEBUG << "Called TrigMuonEFCbTrackCnv_p7::transToPers" << endreq;
  persObj->m_matchChi2    = transObj->m_matchChi2;
  persObj->m_nIdSctHits   = transObj->m_nIdSctHits;
  persObj->m_nIdPixelHits = transObj->m_nIdPixelHits;
  persObj->m_nTrtHits     = transObj->m_nTrtHits;

   trackCnv.transToPers(&transObj->m_idTrackLink, &persObj->m_idTrackLink, log);

  // std::cout << "TrigMuonEFCbTrackCnv_p7::transToPers before base TrackCnv" << std::endl;
  persObj->m_TrigMuonEFTrack = baseToPersistent( &m_TrigMuonEFTrackCnv, transObj, log );
  // std::cout << "TrigMuonEFCbTrackCnv_p7::transToPers after base TrackCnv" << std::endl;
}
