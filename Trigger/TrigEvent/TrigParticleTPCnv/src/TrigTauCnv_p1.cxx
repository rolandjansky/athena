/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigTau.h"
#undef private
#undef protected


#include "TrigParticleTPCnv/TrigTau_p1.h"
#include "TrigParticleTPCnv/TrigTauCnv_p1.h"


void TrigTauCnv_p1::persToTrans(const TrigTau_p1* persObj, 
				       TrigTau* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p1::persToTrans called " << endreq;

   transObj->m_roiID	      = persObj->m_roiID	 ; 
   transObj->m_Zvtx 	      = persObj->m_Zvtx 	 ;
   transObj->m_err_Zvtx	      = persObj->m_err_Zvtx	 ;
   transObj->m_etCalibCluster = persObj->m_etCalibCluster;
   transObj->m_simpleEtFlow         = 0;
   transObj->m_nMatchedTracks = persObj->m_nMatchedTracks; 

   transObj->m_cluster = 0;
   transObj->m_tracks = 0;

   fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );
  
}


void TrigTauCnv_p1::transToPers(const TrigTau* transObj, 
				       TrigTau_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p1::transToPers called " << endreq;

   persObj->m_roiID	      = transObj->m_roiID	  ; 
   persObj->m_Zvtx 	      = transObj->m_Zvtx 	  ;
   persObj->m_err_eta	      = 0;
   persObj->m_err_phi	      = 0;
   persObj->m_err_Zvtx	      = transObj->m_err_Zvtx	  ;
   persObj->m_err_Pt	      = 0;
   persObj->m_etCalibCluster  = transObj->m_etCalibCluster;
   //persObj->m_simpleEtFlow  //OI was introduced after p1 was created
   persObj->m_nMatchedTracks  = transObj->m_nMatchedTracks; 

   persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );
}
