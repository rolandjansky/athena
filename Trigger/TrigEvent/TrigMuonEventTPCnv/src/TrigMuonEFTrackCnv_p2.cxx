/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p2.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p2.h"


void TrigMuonEFTrackCnv_p2::persToTrans(const TrigMuonEFTrack_p2* persObj,
					TrigMuonEFTrack* transObj,
					MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFTrackCnv_p2::persToTrans called " << endreq;

   transObj->m_charge      = persObj->m_charge;
   transObj->m_d0          = persObj->m_d0;
   transObj->m_z0          = persObj->m_z0;
   transObj->m_chi2        = persObj->m_chi2;
   transObj->m_chi2prob    = persObj->m_chi2prob;
   transObj->m_posx        = persObj->m_posx;
   transObj->m_posy        = persObj->m_posy;
   transObj->m_posz        = persObj->m_posz;
   transObj->m_nCscHitsEta = persObj->m_nCscHitsEta;
   transObj->m_nTgcHitsEta = persObj->m_nTgcHitsEta;
   transObj->m_nRpcHitsEta = persObj->m_nRpcHitsEta;
   transObj->m_nMdtHitsEta = persObj->m_nMdtHitsEta;
   transObj->m_nCscHitsPhi = persObj->m_nCscHitsPhi;
   transObj->m_nTgcHitsPhi = persObj->m_nTgcHitsPhi;
   transObj->m_nRpcHitsPhi = persObj->m_nRpcHitsPhi;
   transObj->m_nMdtHitsPhi = persObj->m_nMdtHitsPhi;


   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
}


void TrigMuonEFTrackCnv_p2::transToPers(const TrigMuonEFTrack* /*transObj*/,
					TrigMuonEFTrack_p2* /*persObj*/,
					MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFTrackCnv_p2::transToPers called " << endreq;
//
//  persObj->m_charge      = transObj->m_charge;
//  persObj->m_d0          = transObj->m_d0;
//  persObj->m_z0          = transObj->m_z0;
//  persObj->m_chi2        = transObj->m_chi2;
//  persObj->m_chi2prob    = transObj->m_chi2prob;
//  persObj->m_posx        = transObj->m_posx;
//  persObj->m_posy        = transObj->m_posy;
//  persObj->m_posz        = transObj->m_posz;
//  persObj->m_nCscHitsEta = transObj->m_nCscHitsEta;
//  persObj->m_nTgcHitsEta = transObj->m_nTgcHitsEta;
//  persObj->m_nRpcHitsEta = transObj->m_nRpcHitsEta;
//  persObj->m_nMdtHitsEta = transObj->m_nMdtHitsEta;
//  persObj->m_nCscHitsPhi = transObj->m_nCscHitsPhi;
//  persObj->m_nTgcHitsPhi = transObj->m_nTgcHitsPhi;
//  persObj->m_nRpcHitsPhi = transObj->m_nRpcHitsPhi;
//  persObj->m_nMdtHitsPhi = transObj->m_nMdtHitsPhi;
//
//  persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
}
