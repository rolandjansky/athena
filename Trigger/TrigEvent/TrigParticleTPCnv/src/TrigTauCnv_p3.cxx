/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigTau.h"
#include "TrigParticleTPCnv/TrigTau_p3.h"
#include "TrigParticleTPCnv/TrigTauCnv_p3.h"


void TrigTauCnv_p3::persToTrans(const TrigTau_p3* persObj, 
				       TrigTau* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p3::persToTrans called " << endreq;

   transObj->setRoiId	       (persObj->m_roiID); 
   transObj->setZvtx 	       (persObj->m_Zvtx);
   transObj->setErr_Zvtx       (persObj->m_err_Zvtx);
   transObj->setEtCalibCluster (persObj->m_etCalibCluster);
   transObj->setSimpleEtFlow   (persObj->m_simpleEtFlow);
   transObj->setNMatchedTracks (persObj->m_nMatchedTracks);
   transObj->setTrkAvgDist     (persObj->m_trkAvgDist);
   transObj->setEtOverPtLeadTrk(persObj->m_etOverPtLeadTrk);

   transObj->setTauCluster (nullptr);
   transObj->setTrackCollection (nullptr);
   transObj->setTracksInfo (nullptr);

   //fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );
   m_p4PtEtaPhiMCnv.persToTrans(&(persObj->m_p4PtEtaPhiM), transObj, log);
 
}


void TrigTauCnv_p3::transToPers(const TrigTau* transObj, 
				       TrigTau_p3* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p3::transToPers called " << endreq;

   persObj->m_roiID	      = transObj->roiId();
   persObj->m_Zvtx 	      = transObj->Zvtx();
   persObj->m_err_Zvtx	      = transObj->err_Zvtx();
   persObj->m_etCalibCluster  = transObj->etCalibCluster();
   persObj->m_simpleEtFlow    = transObj->simpleEtFlow();
   persObj->m_nMatchedTracks  = transObj->nMatchedTracks(); 
   persObj->m_trkAvgDist      = transObj->trkAvgDist();
   persObj->m_etOverPtLeadTrk = transObj->etOverPtLeadTrk();
   

   //persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );
   m_p4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_p4PtEtaPhiM), log);  
}
