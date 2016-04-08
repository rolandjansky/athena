/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p6.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p6.h"


void TrigMuonEFTrackCnv_p6::persToTrans(const TrigMuonEFTrack_p6* persObj, TrigMuonEFTrack* transObj, MsgStream &log) {
    
    // std::cout << "TrigMuonEFTrackCnv_p6::persToTrans called " << std::endl;

    transObj->setCharge      (persObj->m_allTheFloats[0]);
    transObj->setD0          (persObj->m_allTheFloats[1]);
    transObj->setZ0          (persObj->m_allTheFloats[2]);
    transObj->setChi2        (persObj->m_allTheFloats[3]);
    transObj->setChi2prob    (persObj->m_allTheFloats[4]);
    transObj->setPosX        (persObj->m_allTheFloats[5]);
    transObj->setPosY        (persObj->m_allTheFloats[6]);
    transObj->setPosZ        (persObj->m_allTheFloats[7]);
    
    
    transObj->setNRpcHitsPhi (persObj->m_allTheInts[0]);
    transObj->setNTgcHitsPhi (persObj->m_allTheInts[1]);
    transObj->setNCscHitsPhi (persObj->m_allTheInts[2]);
    transObj->setNRpcHitsEta (persObj->m_allTheInts[3]);
    transObj->setNTgcHitsEta (persObj->m_allTheInts[4]);
    transObj->setNCscHitsEta (persObj->m_allTheInts[5]);
    transObj->setNMdtHits    (persObj->m_allTheInts[6]);

   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
}


void TrigMuonEFTrackCnv_p6::transToPers(const TrigMuonEFTrack* transObj, TrigMuonEFTrack_p6* persObj, MsgStream &log) {
    // std::cout << "TrigMuonEFTrackCnv_p6::transToPers called " << std::endl;

  persObj->m_allTheFloats[0] = transObj->charge();
  persObj->m_allTheFloats[1] = transObj->d0();
  persObj->m_allTheFloats[2] = transObj->z0();
  persObj->m_allTheFloats[3] = transObj->chi2();
  persObj->m_allTheFloats[4] = transObj->chi2prob();
  persObj->m_allTheFloats[5] = transObj->posX();
  persObj->m_allTheFloats[6] = transObj->posY();
  persObj->m_allTheFloats[7] = transObj->posZ();

  persObj->m_allTheInts[0]   = transObj->NRpcHitsPhi();
  persObj->m_allTheInts[1]   = transObj->NTgcHitsPhi();
  persObj->m_allTheInts[2]   = transObj->NCscHitsPhi();
  persObj->m_allTheInts[3]   = transObj->NRpcHitsEta();
  persObj->m_allTheInts[4]   = transObj->NTgcHitsEta();
  persObj->m_allTheInts[5]   = transObj->NCscHitsEta();
  persObj->m_allTheInts[6]   = transObj->NMdtHits();
  
  // std::cout << "TrigMuonEFTrackCnv_p6::transToPers before P4Cnv " << std::endl;
  persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
  // std::cout << "TrigMuonEFTrackCnv_p6::transToPers after P4Cnv " << std::endl;
}
