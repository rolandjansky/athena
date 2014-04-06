/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigTau.h"
#undef private
#undef protected


#include "TrigParticleTPCnv/TrigTau_p3.h"
#include "TrigParticleTPCnv/TrigTauCnv_p3.h"


void TrigTauCnv_p3::persToTrans(const TrigTau_p3* persObj, 
				       TrigTau* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p3::persToTrans called " << endreq;

   transObj->m_roiID	      = persObj->m_roiID	 ; 
   transObj->m_Zvtx 	      = persObj->m_Zvtx 	 ;
   transObj->m_err_Zvtx	      = persObj->m_err_Zvtx	 ;
   transObj->m_etCalibCluster = persObj->m_etCalibCluster;
   transObj->m_simpleEtFlow         = persObj->m_simpleEtFlow;
   transObj->m_nMatchedTracks = persObj->m_nMatchedTracks; 
   transObj->m_trkAvgDist = persObj->m_trkAvgDist;
   transObj->m_etOverPtLeadTrk = persObj->m_etOverPtLeadTrk;
   transObj->m_cluster = 0;
   transObj->m_tracks = 0;

   //fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );
   m_p4PtEtaPhiMCnv.persToTrans(&(persObj->m_p4PtEtaPhiM), transObj, log);
 
}


void TrigTauCnv_p3::transToPers(const TrigTau* transObj, 
				       TrigTau_p3* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p3::transToPers called " << endreq;

   persObj->m_roiID	      = transObj->m_roiID	  ; 
   persObj->m_Zvtx 	      = transObj->m_Zvtx 	  ;
   persObj->m_err_Zvtx	      = transObj->m_err_Zvtx	  ;
   persObj->m_etCalibCluster  = transObj->m_etCalibCluster;
   persObj->m_simpleEtFlow    = transObj->m_simpleEtFlow;
   persObj->m_nMatchedTracks  = transObj->m_nMatchedTracks; 
   persObj->m_trkAvgDist      = transObj->m_trkAvgDist;
   persObj->m_etOverPtLeadTrk = transObj->m_etOverPtLeadTrk;
   

   //persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );
   m_p4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_p4PtEtaPhiM), log);  
}
