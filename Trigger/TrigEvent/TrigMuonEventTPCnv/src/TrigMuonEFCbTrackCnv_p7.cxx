/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p7.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p7.h"

typedef ElementLinkCnv_p3<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;

/// pre-allocate the track converter
static TrackLinkCnv_t         trackCnv;

void TrigMuonEFCbTrackCnv_p7::persToTrans(const TrigMuonEFCbTrack_p7* persObj, TrigMuonEFCbTrack* transObj, MsgStream &log){
    // std::cout << "TrigMuonEFCbTrackCnv_p7::persToTrans called " <<std::endl;

  //log << MSG::DEBUG << "Called TrigMuonEFCbTrackCnv_p7::persToTrans" << endreq;
  transObj->setMatchChi2    (persObj->m_matchChi2);
  transObj->setNIdSctHits   (persObj->m_nIdSctHits);
  transObj->setNIdPixelHits (persObj->m_nIdPixelHits);
  transObj->setNTrtHits     (persObj->m_nTrtHits);

  ElementLink<Rec::TrackParticleContainer> idTrackLink;
  trackCnv.persToTrans(&persObj->m_idTrackLink, &idTrackLink, log);
  transObj->setIDTrackElementLink (idTrackLink);

   fillTransFromPStore( &m_TrigMuonEFTrackCnv, persObj->m_TrigMuonEFTrack, transObj, log );
}


void TrigMuonEFCbTrackCnv_p7::transToPers(const TrigMuonEFCbTrack* transObj, TrigMuonEFCbTrack_p7* persObj, MsgStream &log) {
    // std::cout << "TrigMuonEFCbTrackCnv_p7::transToPers called " << std::endl;

  //log << MSG::DEBUG << "Called TrigMuonEFCbTrackCnv_p7::transToPers" << endreq;
  persObj->m_matchChi2    = transObj->matchChi2();
  persObj->m_nIdSctHits   = transObj->NIdSctHits();
  persObj->m_nIdPixelHits = transObj->NIdPixelHits();
  persObj->m_nTrtHits     = transObj->NTrtHits();

  trackCnv.transToPers(&transObj->getIDTrackParticleLink(), &persObj->m_idTrackLink, log);

  // std::cout << "TrigMuonEFCbTrackCnv_p7::transToPers before base TrackCnv" << std::endl;
  persObj->m_TrigMuonEFTrack = baseToPersistent( &m_TrigMuonEFTrackCnv, transObj, log );
  // std::cout << "TrigMuonEFCbTrackCnv_p7::transToPers after base TrackCnv" << std::endl;
}
