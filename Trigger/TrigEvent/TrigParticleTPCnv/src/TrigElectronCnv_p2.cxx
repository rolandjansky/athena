/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigElectron.h"
#include "TrigParticleTPCnv/TrigElectron_p2.h"
#include "TrigParticleTPCnv/TrigElectronCnv_p2.h"


static const ElementLinkCnv_p1<ElementLink<TrigInDetTrackCollection> > ELinkIDTrackCnv;
// static ElementLinkCnv_p1<ElementLink<TrigEMClusterContainer> >   m_ELinkEMclusterCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigElectronCnv_p2::persToTrans(const TrigElectron_p2* persObj, 
				       TrigElectron* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigElectronCnv_p2::persToTrans called " << endmsg;

   ElementLink< TrigInDetTrackCollection > track;
   ELinkIDTrackCnv.persToTrans( &persObj->m_track,     &track,   log );

   ElementLink< TrigEMClusterContainer > cluster;
   m_ELinkEMclusterCnv.persToTrans( &persObj->m_cluster, &cluster, log );

   *transObj = TrigElectron (0, 0, 0,
                             persObj->m_roiWord,
                             persObj->m_valid,
                             persObj->m_tr_eta_at_calo,  // trkEtaAtCalo
                             persObj->m_tr_phi_at_calo,  // trkPhiAtCalo
                             persObj->m_etoverpt,        // EToverPT,
                             cluster,
                             persObj->m_cl_eta,          // caloEta
                             persObj->m_cl_phi,          // caloPhi
                             persObj->m_cl_Rcore,        // Rcore
                             persObj->m_cl_Eratio,       // Eratio
                             persObj->m_cl_EThad,        // EThad
                             persObj->m_cl_e_frac_S0,    // F0
                             persObj->m_cl_e_frac_S1,    // F1
                             persObj->m_cl_e_frac_S2,    // F2
                             persObj->m_cl_e_frac_S3,    // F3
                             track,
                             persObj->m_tr_Algo, // trackAlgo
                             persObj->m_tr_Zvtx,
                             persObj->m_tr_nr_trt_hits,
                             persObj->m_tr_nr_trt_hithresh_hits);

   log << MSG::DEBUG << "TrigElectronCnv_p2::persToTrans filling 4-mom. base class from persistent" << endmsg;
   fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );

   log << MSG::DEBUG << "TrigElectronCnv_p2::persToTrans done" << endmsg;
}


//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigElectronCnv_p2::transToPers(const TrigElectron* transObj, 
				       TrigElectron_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigElectronCnv_p2::transToPers called " << endmsg;

   persObj->m_roiWord        = transObj->roiId()   ; 
   persObj->m_valid          = transObj->isValid()     ; 
   persObj->m_tr_Algo        = transObj->trackAlgo()   ; 
   persObj->m_tr_Zvtx        = transObj->Zvtx()   ; 
   persObj->m_tr_nr_trt_hits = transObj->nTRTHits()  ;
   persObj->m_tr_nr_trt_hithresh_hits  =  transObj->nTRTHiThresholdHits()  ;
   persObj->m_tr_eta_at_calo = transObj->trkEtaAtCalo();
   persObj->m_tr_phi_at_calo = transObj->trkPhiAtCalo();
   persObj->m_etoverpt       = transObj->EtOverPt()  ;
   persObj->m_cl_eta         = transObj->caloEta();
   persObj->m_cl_phi         = transObj->caloPhi();
   persObj->m_cl_Rcore       = transObj->Rcore()  ;  
   persObj->m_cl_Eratio      = transObj->Eratio() ; 
   persObj->m_cl_EThad       = transObj->EThad()  ;
   persObj->m_cl_e_frac_S0   = transObj->F0();
   persObj->m_cl_e_frac_S1   = transObj->F1();
   persObj->m_cl_e_frac_S2   = transObj->F2();
   persObj->m_cl_e_frac_S3   = transObj->F3();

   log << MSG::DEBUG << "TrigElectronCnv_p2::transToPers calling e-link persToTrabs(track) " << endmsg;   
   ELinkIDTrackCnv.transToPers( &transObj->trackLink(),     &persObj->m_track,   log );
   log << MSG::DEBUG << "TrigElectronCnv_p2::transToPers calling e-link persToTrabs(cluster) " << endmsg;   
   m_ELinkEMclusterCnv.transToPers( &transObj->clusterLink(), &persObj->m_cluster, log );

   log << MSG::DEBUG << "TrigElectronCnv_p2::transToPers persistifying 4-mom. base class" << endmsg;   
   persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );

   log << MSG::DEBUG << "TrigElectronCnv_p1::transToPers done" << endmsg;
}
