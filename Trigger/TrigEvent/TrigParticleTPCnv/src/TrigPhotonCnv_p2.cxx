/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigPhoton.h"
#include "TrigParticleTPCnv/TrigPhoton_p2.h"
#include "TrigParticleTPCnv/TrigPhotonCnv_p2.h"
 
static ElementLinkCnv_p1< ElementLink<TrigEMClusterContainer> > eLinkEMClusterCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p2::persToTrans( const TrigPhoton_p2 *persObj,
				    TrigPhoton    *transObj,
				    MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p2::persToTrans called " << endreq;

  ElementLink< TrigEMClusterContainer > cluster;
  eLinkEMClusterCnv.persToTrans( &persObj->m_cluster, &cluster, log ); 

  *transObj = TrigPhoton (0, 0, 0,
                          persObj->m_roiID,
                          persObj->m_valid,
                          cluster,
                          persObj->m_HadEt,
                          persObj->m_energyRatio,
                          persObj->m_rCore,
                          persObj->m_dEta,
                          persObj->m_dPhi,
                          persObj->m_Fside,
                          persObj->m_Weta2,
                          persObj->m_cl_e_frac_S0,
                          persObj->m_cl_e_frac_S1,
                          persObj->m_cl_e_frac_S2,
                          persObj->m_cl_e_frac_S3
                          );

  fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );     
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p2::transToPers( const TrigPhoton    *transObj,
				    TrigPhoton_p2 *persObj,
				    MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p2::transToPers called " << endreq;

  persObj->m_roiID        = transObj->roiId()        ;
  persObj->m_HadEt        = transObj->HadEt()        ;
  persObj->m_energyRatio  = transObj->energyRatio()  ;
  persObj->m_rCore        = transObj->rCore()        ;
  persObj->m_dPhi         = transObj->dPhi()         ;
  persObj->m_dEta         = transObj->dEta()         ;
  persObj->m_cl_e_frac_S0 = transObj->F0() ;
  persObj->m_cl_e_frac_S1 = transObj->F1() ;
  persObj->m_cl_e_frac_S2 = transObj->F2() ;
  persObj->m_cl_e_frac_S3 = transObj->F3() ;
  persObj->m_Fside        = transObj->Fside() ;
  persObj->m_Weta2        = transObj->Weta2() ;
  persObj->m_valid        = transObj->isValid()        ;

  eLinkEMClusterCnv.transToPers( &transObj->clusterLink(), &persObj->m_cluster, log );

  persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );  
    
}
