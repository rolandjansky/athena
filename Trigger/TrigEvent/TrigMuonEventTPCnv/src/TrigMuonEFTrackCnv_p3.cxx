/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p3.h"


void TrigMuonEFTrackCnv_p3::persToTrans(const TrigMuonEFTrack_p3* persObj,
					TrigMuonEFTrack* transObj,
					MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFTrackCnv_p3::persToTrans called " << endreq;

   transObj->setCharge      (persObj->m_charge);
   transObj->setD0          (persObj->m_d0);
   transObj->setZ0          (persObj->m_z0);
   transObj->setChi2        (persObj->m_chi2);
   transObj->setChi2prob    (persObj->m_chi2prob);
   transObj->setPosX        (persObj->m_posx);
   transObj->setPosY        (persObj->m_posy);
   transObj->setPosZ        (persObj->m_posz);
   transObj->setNCscHitsEta (persObj->m_nCscHitsEta);
   transObj->setNTgcHitsEta (persObj->m_nTgcHitsEta);
   transObj->setNRpcHitsEta (persObj->m_nRpcHitsEta);
   transObj->setNCscHitsPhi (persObj->m_nCscHitsPhi);
   transObj->setNTgcHitsPhi (persObj->m_nTgcHitsPhi);
   transObj->setNRpcHitsPhi (persObj->m_nRpcHitsPhi);

   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
}


void TrigMuonEFTrackCnv_p3::transToPers(const TrigMuonEFTrack* /*transObj*/,
					TrigMuonEFTrack_p3* /*persObj*/,
					MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFTrackCnv_p3::transToPers called " << endreq;

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
//  persObj->m_nCscHitsPhi = transObj->m_nCscHitsPhi;
//  persObj->m_nTgcHitsPhi = transObj->m_nTgcHitsPhi;
//  persObj->m_nRpcHitsPhi = transObj->m_nRpcHitsPhi;

//  persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
}
