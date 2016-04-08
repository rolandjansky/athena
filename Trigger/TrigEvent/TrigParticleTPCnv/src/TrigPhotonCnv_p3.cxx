/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigPhoton.h"
#include "TrigParticleTPCnv/TrigPhoton_p3.h"
#include "TrigParticleTPCnv/TrigPhotonCnv_p3.h"
 
static ElementLinkCnv_p3< ElementLink<TrigEMClusterContainer> > eLinkEMClusterCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p3::persToTrans( const TrigPhoton_p3 *persObj,
				    TrigPhoton    *transObj,
				    MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p3::persToTrans called " << endreq;

  ElementLink< TrigEMClusterContainer > cluster;
  eLinkEMClusterCnv.persToTrans( &persObj->m_cluster, &cluster, log ); 

  *transObj = TrigPhoton (0, 0, 0,
                          persObj->m_roiID,
                          persObj->m_valid,
                          cluster,
                          persObj->m_allTheFloats[0], // HadEt
                          persObj->m_allTheFloats[1], // energyRatio
                          persObj->m_allTheFloats[2], // rCore,
                          persObj->m_allTheFloats[4], // deta
                          persObj->m_allTheFloats[3], // dphi
                          persObj->m_allTheFloats[9], // Fside
                          persObj->m_allTheFloats[10], // Weta2,
                          persObj->m_allTheFloats[5], // F0
                          persObj->m_allTheFloats[6], // F1
                          persObj->m_allTheFloats[7], // F2
                          persObj->m_allTheFloats[8]  // F3
                          );
  //fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );     
  m_p4PtEtaPhiMCnv.persToTrans(&(persObj->m_P4PtEtaPhiM), transObj, log);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p3::transToPers( const TrigPhoton    *transObj,
				    TrigPhoton_p3 *persObj,
				    MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p3::transToPers called " << endreq;

  persObj->m_roiID        = transObj->roiId();

  persObj->m_allTheFloats[0] = transObj->HadEt()        ;
  persObj->m_allTheFloats[1] = transObj->energyRatio()  ;
  persObj->m_allTheFloats[2] = transObj->rCore()        ;
  persObj->m_allTheFloats[3] = transObj->dPhi()         ;
  persObj->m_allTheFloats[4] = transObj->dEta()         ;
  persObj->m_allTheFloats[5] = transObj->F0() ;
  persObj->m_allTheFloats[6] = transObj->F1() ;
  persObj->m_allTheFloats[7] = transObj->F2() ;
  persObj->m_allTheFloats[8] = transObj->F3() ;
  persObj->m_allTheFloats[9] = transObj->Fside() ;
  persObj->m_allTheFloats[10] = transObj->Weta2() ;

  persObj->m_valid        = transObj->isValid();

  eLinkEMClusterCnv.transToPers( &transObj->clusterLink(), &persObj->m_cluster, log );

  //persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );  
  m_p4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_P4PtEtaPhiM), log);  
}
