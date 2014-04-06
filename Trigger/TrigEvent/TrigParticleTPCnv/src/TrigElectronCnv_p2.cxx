/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigElectron.h"
#include "TrigParticleTPCnv/TrigElectron_p2.h"
#include "TrigParticleTPCnv/TrigElectronCnv_p2.h"
#undef private
#undef protected


static ElementLinkCnv_p1<ElementLink<TrigInDetTrackCollection> > m_ELinkIDTrackCnv;
// static ElementLinkCnv_p1<ElementLink<TrigEMClusterContainer> >   m_ELinkEMclusterCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigElectronCnv_p2::persToTrans(const TrigElectron_p2* persObj, 
				       TrigElectron* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigElectronCnv_p2::persToTrans called " << endreq;

   transObj->m_roiWord        = persObj->m_roiWord   ; 
   transObj->m_valid          = persObj->m_valid     ; 
   transObj->m_tr_Algo        = persObj->m_tr_Algo   ; 
   transObj->m_tr_Zvtx        = persObj->m_tr_Zvtx   ; 
   transObj->m_tr_nr_trt_hits = persObj->m_tr_nr_trt_hits  ;
   transObj->m_tr_nr_trt_hithresh_hits  =  persObj->m_tr_nr_trt_hithresh_hits   ;
   transObj->m_tr_eta_at_calo = persObj->m_tr_eta_at_calo ;  
   transObj->m_tr_phi_at_calo = persObj->m_tr_phi_at_calo ;
   transObj->m_etoverpt       = persObj->m_etoverpt  ;
   transObj->m_cl_eta         = persObj->m_cl_eta    ; 
   transObj->m_cl_phi         = persObj->m_cl_phi    ;
   transObj->m_cl_Rcore       = persObj->m_cl_Rcore  ;  
   transObj->m_cl_Eratio      = persObj->m_cl_Eratio ; 
   transObj->m_cl_EThad       = persObj->m_cl_EThad  ;
   transObj->m_cl_e_frac_S0   = persObj->m_cl_e_frac_S0  ;
   transObj->m_cl_e_frac_S1   = persObj->m_cl_e_frac_S1  ;
   transObj->m_cl_e_frac_S2   = persObj->m_cl_e_frac_S2  ;
   transObj->m_cl_e_frac_S3   = persObj->m_cl_e_frac_S3  ;
   
   log << MSG::DEBUG << "TrigElectronCnv_p2::persToTrans calling e-link persToTrans(track) " << endreq;
   m_ELinkIDTrackCnv.persToTrans( &persObj->m_track,     &transObj->m_track,   log );
   log << MSG::DEBUG << "TrigElectronCnv_p2::persToTrans calling e-link persToTrans(cluster) " << endreq;
   m_ELinkEMclusterCnv.persToTrans( &persObj->m_cluster, &transObj->m_cluster, log );

   log << MSG::DEBUG << "TrigElectronCnv_p2::persToTrans filling 4-mom. base class from persistent" << endreq;
   fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );

   log << MSG::DEBUG << "TrigElectronCnv_p2::persToTrans done" << endreq;
}


//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigElectronCnv_p2::transToPers(const TrigElectron* transObj, 
				       TrigElectron_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigElectronCnv_p2::transToPers called " << endreq;

   persObj->m_roiWord        = transObj->m_roiWord   ; 
   persObj->m_valid          = transObj->m_valid     ; 
   persObj->m_tr_Algo        = transObj->m_tr_Algo   ; 
   persObj->m_tr_Zvtx        = transObj->m_tr_Zvtx   ; 
   persObj->m_tr_nr_trt_hits = transObj->m_tr_nr_trt_hits  ;
   persObj->m_tr_nr_trt_hithresh_hits  =  transObj->m_tr_nr_trt_hithresh_hits  ;
   persObj->m_tr_eta_at_calo = transObj->m_tr_eta_at_calo ;  
   persObj->m_tr_phi_at_calo = transObj->m_tr_phi_at_calo ;
   persObj->m_etoverpt       = transObj->m_etoverpt  ;
   persObj->m_cl_eta         = transObj->m_cl_eta    ; 
   persObj->m_cl_phi         = transObj->m_cl_phi    ;
   persObj->m_cl_Rcore       = transObj->m_cl_Rcore  ;  
   persObj->m_cl_Eratio      = transObj->m_cl_Eratio ; 
   persObj->m_cl_EThad       = transObj->m_cl_EThad  ;
   persObj->m_cl_e_frac_S0   = transObj->m_cl_e_frac_S0  ;
   persObj->m_cl_e_frac_S1   = transObj->m_cl_e_frac_S1  ;
   persObj->m_cl_e_frac_S2   = transObj->m_cl_e_frac_S2  ;
   persObj->m_cl_e_frac_S3   = transObj->m_cl_e_frac_S3  ;

   log << MSG::DEBUG << "TrigElectronCnv_p2::transToPers calling e-link persToTrabs(track) " << endreq;   
   m_ELinkIDTrackCnv.transToPers( &transObj->m_track,     &persObj->m_track,   log );
   log << MSG::DEBUG << "TrigElectronCnv_p2::transToPers calling e-link persToTrabs(cluster) " << endreq;   
   m_ELinkEMclusterCnv.transToPers( &transObj->m_cluster, &persObj->m_cluster, log );

   log << MSG::DEBUG << "TrigElectronCnv_p2::transToPers persistifying 4-mom. base class" << endreq;   
   persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );

   log << MSG::DEBUG << "TrigElectronCnv_p1::transToPers done" << endreq;
}
