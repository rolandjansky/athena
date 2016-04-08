/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigTau.h"
#include "TrigParticleTPCnv/TrigTau_p1.h"
#include "TrigParticleTPCnv/TrigTauCnv_p1.h"


void TrigTauCnv_p1::persToTrans(const TrigTau_p1* persObj, 
				       TrigTau* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p1::persToTrans called " << endreq;

   transObj->setRoiId	       (persObj->m_roiID); 
   transObj->setZvtx 	       (persObj->m_Zvtx);
   transObj->setErr_Zvtx       (persObj->m_err_Zvtx);
   transObj->setEtCalibCluster (persObj->m_etCalibCluster);
   transObj->setSimpleEtFlow   (0);
   transObj->setNMatchedTracks (persObj->m_nMatchedTracks);

   transObj->setTauCluster (nullptr);
   transObj->setTrackCollection (nullptr);

   fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );
}


void TrigTauCnv_p1::transToPers(const TrigTau* transObj, 
				       TrigTau_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p1::transToPers called " << endreq;

   persObj->m_roiID	      = transObj->roiId()	  ; 
   persObj->m_Zvtx 	      = transObj->Zvtx() 	  ;
   persObj->m_err_eta	      = 0;
   persObj->m_err_phi	      = 0;
   persObj->m_err_Zvtx	      = transObj->err_Zvtx()	  ;
   persObj->m_err_Pt	      = 0;
   persObj->m_etCalibCluster  = transObj->etCalibCluster();
   //persObj->m_simpleEtFlow  //OI was introduced after p1 was created
   persObj->m_nMatchedTracks  = transObj->nMatchedTracks(); 

   persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );
}
