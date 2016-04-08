/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p5.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p5.h"


void TrigMuonEFTrackCnv_p5::persToTrans(const TrigMuonEFTrack_p5* persObj,
					TrigMuonEFTrack* transObj,
					MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFTrackCnv_p5::persToTrans called " << endreq;

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
   transObj->setNMdtHits    (persObj->m_nMdtHits);

   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
}


void TrigMuonEFTrackCnv_p5::transToPers(const TrigMuonEFTrack* transObj,
					TrigMuonEFTrack_p5* persObj,
					MsgStream &log)
{
  log << MSG::DEBUG << "TrigMuonEFTrackCnv_p5::transToPers called " << endreq;

  persObj->m_charge      = transObj->charge();
  persObj->m_d0          = transObj->d0();
  persObj->m_z0          = transObj->z0();
  persObj->m_chi2        = transObj->chi2();
  persObj->m_chi2prob    = transObj->chi2prob();
  persObj->m_posx        = transObj->posX();
  persObj->m_posy        = transObj->posY();
  persObj->m_posz        = transObj->posZ();
  persObj->m_nCscHitsEta = transObj->NCscHitsEta();
  persObj->m_nTgcHitsEta = transObj->NTgcHitsEta();
  persObj->m_nRpcHitsEta = transObj->NRpcHitsEta();
  persObj->m_nCscHitsPhi = transObj->NCscHitsPhi();
  persObj->m_nTgcHitsPhi = transObj->NTgcHitsPhi();
  persObj->m_nRpcHitsPhi = transObj->NRpcHitsPhi();
  persObj->m_nMdtHits    = transObj->NMdtHits();

  persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
}
