/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p6.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p6.h"


void TrigMuonEFTrackCnv_p6::persToTrans(const TrigMuonEFTrack_p6* persObj, TrigMuonEFTrack* transObj, MsgStream &log) {
    
    // std::cout << "TrigMuonEFTrackCnv_p6::persToTrans called " << std::endl;

    transObj->m_charge      = persObj->m_allTheFloats[0];
    transObj->m_d0          = persObj->m_allTheFloats[1];
    transObj->m_z0          = persObj->m_allTheFloats[2];
    transObj->m_chi2        = persObj->m_allTheFloats[3];
    transObj->m_chi2prob    = persObj->m_allTheFloats[4];
    transObj->m_posx        = persObj->m_allTheFloats[5];
    transObj->m_posy        = persObj->m_allTheFloats[6];
    transObj->m_posz        = persObj->m_allTheFloats[7];
    
    
    transObj->m_nRpcHitsPhi = persObj->m_allTheInts[0];
    transObj->m_nTgcHitsPhi = persObj->m_allTheInts[1];
    transObj->m_nCscHitsPhi = persObj->m_allTheInts[2];
    transObj->m_nRpcHitsEta = persObj->m_allTheInts[3];
    transObj->m_nTgcHitsEta = persObj->m_allTheInts[4];
    transObj->m_nCscHitsEta = persObj->m_allTheInts[5];
    transObj->m_nMdtHits    = persObj->m_allTheInts[6];

   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
}


void TrigMuonEFTrackCnv_p6::transToPers(const TrigMuonEFTrack* transObj, TrigMuonEFTrack_p6* persObj, MsgStream &log) {
    // std::cout << "TrigMuonEFTrackCnv_p6::transToPers called " << std::endl;

  persObj->m_allTheFloats[0] = transObj->m_charge;
  persObj->m_allTheFloats[1] = transObj->m_d0;
  persObj->m_allTheFloats[2] = transObj->m_z0;
  persObj->m_allTheFloats[3] = transObj->m_chi2;
  persObj->m_allTheFloats[4] = transObj->m_chi2prob;
  persObj->m_allTheFloats[5] = transObj->m_posx;
  persObj->m_allTheFloats[6] = transObj->m_posy;
  persObj->m_allTheFloats[7] = transObj->m_posz;

  persObj->m_allTheInts[0]   = transObj->m_nRpcHitsPhi;
  persObj->m_allTheInts[1]   = transObj->m_nTgcHitsPhi;
  persObj->m_allTheInts[2]   = transObj->m_nCscHitsPhi;
  persObj->m_allTheInts[3]   = transObj->m_nRpcHitsEta;
  persObj->m_allTheInts[4]   = transObj->m_nTgcHitsEta;
  persObj->m_allTheInts[5]   = transObj->m_nCscHitsEta;
  persObj->m_allTheInts[6]   = transObj->m_nMdtHits;
  
  // std::cout << "TrigMuonEFTrackCnv_p6::transToPers before P4Cnv " << std::endl;
  persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
  // std::cout << "TrigMuonEFTrackCnv_p6::transToPers after P4Cnv " << std::endl;
}
