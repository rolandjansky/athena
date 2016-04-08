/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigTau.h"
#include "TrigParticleTPCnv/TrigTau_p2.h"
#include "TrigParticleTPCnv/TrigTauCnv_p2.h"


void TrigTauCnv_p2::persToTrans(const TrigTau_p2* persObj, 
				       TrigTau* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p2::persToTrans called " << endreq;

   transObj->setRoiId	       (persObj->m_roiID); 
   transObj->setZvtx 	       (persObj->m_Zvtx);
   transObj->setErr_Zvtx       (persObj->m_err_Zvtx);
   transObj->setEtCalibCluster (persObj->m_etCalibCluster);
   transObj->setSimpleEtFlow   (persObj->m_simpleEtFlow);
   transObj->setNMatchedTracks (persObj->m_nMatchedTracks);

   transObj->setTauCluster (nullptr);
   transObj->setTrackCollection (nullptr);

   fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );
  
}


void TrigTauCnv_p2::transToPers(const TrigTau* transObj, 
				       TrigTau_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p2::transToPers called " << endreq;

   persObj->m_roiID	      = transObj->roiId();
   persObj->m_Zvtx 	      = transObj->Zvtx();
   persObj->m_err_Zvtx	      = transObj->err_Zvtx();
   persObj->m_etCalibCluster  = transObj->etCalibCluster();
   persObj->m_simpleEtFlow    = transObj->simpleEtFlow();
   persObj->m_nMatchedTracks  = transObj->nMatchedTracks(); 

   persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );
}
