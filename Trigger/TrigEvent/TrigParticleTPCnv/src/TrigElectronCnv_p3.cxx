/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigElectron.h"
#include "TrigParticleTPCnv/TrigElectron_p3.h"
#include "TrigParticleTPCnv/TrigElectronCnv_p3.h"


static ElementLinkCnv_p3<ElementLink<TrigInDetTrackCollection> > ELinkIDTrackCnv;
// static ElementLinkCnv_p1<ElementLink<TrigEMClusterContainer> >   m_ELinkEMclusterCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigElectronCnv_p3::persToTrans(const TrigElectron_p3* persObj, 
				       TrigElectron* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigElectronCnv_p3::persToTrans called " << endreq;

   ElementLink< TrigInDetTrackCollection > track;
   ELinkIDTrackCnv.persToTrans( &persObj->m_track,     &track,   log );

   ElementLink< TrigEMClusterContainer > cluster;
   m_ELinkEMclusterCnv.persToTrans( &persObj->m_cluster, &cluster, log );

   *transObj = TrigElectron (0, 0, 0,
                             persObj->m_roiWord,
                             persObj->m_valid,
                             persObj->m_allTheFloats[1], // trkEtaAtCalo
                             persObj->m_allTheFloats[2], // trkPhiAtCalo
                             persObj->m_allTheFloats[3], // EToverPT,
                             cluster,
                             persObj->m_allTheFloats[4], // caloEta
                             persObj->m_allTheFloats[5], // caloPhi
                             persObj->m_allTheFloats[6], // Rcore
                             persObj->m_allTheFloats[7], // Eratio
                             persObj->m_allTheFloats[8], // EThad
                             persObj->m_allTheFloats[9], // F0
                             persObj->m_allTheFloats[10], // F1
                             persObj->m_allTheFloats[11], // F2
                             persObj->m_allTheFloats[12], // F3
                             track,
                             persObj->m_tr_Algo, // trackAlgo
                             persObj->m_allTheFloats[0], // Zvtx
                             persObj->m_tr_nr_trt_hits,
                             persObj->m_tr_nr_trt_hithresh_hits);



   m_p4PtEtaPhiMCnv.persToTrans(&(persObj->m_P4PtEtaPhiM), transObj, log);
   //fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );

   log << MSG::DEBUG << "TrigElectronCnv_p3::persToTrans done" << endreq;
}


//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigElectronCnv_p3::transToPers(const TrigElectron* transObj, 
				       TrigElectron_p3* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigElectronCnv_p3::transToPers called " << endreq;

   persObj->m_roiWord        = transObj->roiId()   ; 
   persObj->m_valid          = transObj->isValid()     ; 
   persObj->m_tr_Algo        = transObj->trackAlgo()   ; 
   persObj->m_tr_nr_trt_hits = transObj->nTRTHits()  ;
   persObj->m_tr_nr_trt_hithresh_hits  =  transObj->nTRTHiThresholdHits()  ;

   persObj->m_allTheFloats[0]  = transObj->Zvtx()   ; 
   persObj->m_allTheFloats[1]  = transObj->trkEtaAtCalo() ;  
   persObj->m_allTheFloats[2]  = transObj->trkPhiAtCalo() ;  
   persObj->m_allTheFloats[3]  = transObj->EtOverPt()  ;
   persObj->m_allTheFloats[4]  = transObj->caloEta();
   persObj->m_allTheFloats[5]  = transObj->caloPhi();
   persObj->m_allTheFloats[6]  = transObj->Rcore()  ;  
   persObj->m_allTheFloats[7]  = transObj->Eratio() ; 
   persObj->m_allTheFloats[8]  = transObj->EThad()  ;
   persObj->m_allTheFloats[9]  = transObj->F0()  ;
   persObj->m_allTheFloats[10] = transObj->F1()  ;
   persObj->m_allTheFloats[11] = transObj->F2()  ;
   persObj->m_allTheFloats[12] = transObj->F3()  ;

   log << MSG::DEBUG << "TrigElectronCnv_p3::transToPers calling e-link persToTrabs(track) " << endreq;   
   ELinkIDTrackCnv.transToPers( &transObj->trackLink(),     &persObj->m_track,   log );
   log << MSG::DEBUG << "TrigElectronCnv_p3::transToPers calling e-link persToTrabs(cluster) " << endreq;   
   m_ELinkEMclusterCnv.transToPers( &transObj->clusterLink(), &persObj->m_cluster, log );

   log << MSG::DEBUG << "TrigElectronCnv_p3::transToPers persistifying 4-mom. base class" << endreq;   
   m_p4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_P4PtEtaPhiM), log);
   //persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );

   log << MSG::DEBUG << "TrigElectronCnv_p3::transToPers done" << endreq;
}
