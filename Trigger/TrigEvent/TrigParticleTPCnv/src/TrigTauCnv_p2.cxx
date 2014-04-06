/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigTau.h"
#undef private
#undef protected


#include "TrigParticleTPCnv/TrigTau_p2.h"
#include "TrigParticleTPCnv/TrigTauCnv_p2.h"


void TrigTauCnv_p2::persToTrans(const TrigTau_p2* persObj, 
				       TrigTau* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p2::persToTrans called " << endreq;

   transObj->m_roiID	      = persObj->m_roiID	 ; 
   transObj->m_Zvtx 	      = persObj->m_Zvtx 	 ;
   transObj->m_err_Zvtx	      = persObj->m_err_Zvtx	 ;
   transObj->m_etCalibCluster = persObj->m_etCalibCluster;
   transObj->m_simpleEtFlow         = persObj->m_simpleEtFlow;
   transObj->m_nMatchedTracks = persObj->m_nMatchedTracks; 
   transObj->m_cluster = 0;
   transObj->m_tracks = 0;

   fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );
  
}


void TrigTauCnv_p2::transToPers(const TrigTau* transObj, 
				       TrigTau_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigTauCnv_p2::transToPers called " << endreq;

   persObj->m_roiID	      = transObj->m_roiID	  ; 
   persObj->m_Zvtx 	      = transObj->m_Zvtx 	  ;
   persObj->m_err_Zvtx	      = transObj->m_err_Zvtx	  ;
   persObj->m_etCalibCluster  = transObj->m_etCalibCluster;
   persObj->m_simpleEtFlow          = transObj->m_simpleEtFlow;
   persObj->m_nMatchedTracks  = transObj->m_nMatchedTracks; 

   persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );
}
