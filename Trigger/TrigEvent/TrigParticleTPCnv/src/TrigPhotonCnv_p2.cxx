/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigPhoton.h"
#include "TrigParticleTPCnv/TrigPhoton_p2.h"
#undef private
#undef protected
 
#include "TrigParticleTPCnv/TrigPhotonCnv_p2.h"
 
static ElementLinkCnv_p1< ElementLink<TrigEMClusterContainer> > m_eLinkEMClusterCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p2::persToTrans( const TrigPhoton_p2 *persObj,
				    TrigPhoton    *transObj,
				    MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p2::persToTrans called " << endreq;

  transObj->m_roiID        = persObj->m_roiID        ;
  transObj->m_HadEt        = persObj->m_HadEt        ;
  transObj->m_energyRatio  = persObj->m_energyRatio  ;
  transObj->m_rCore        = persObj->m_rCore        ;
  transObj->m_dPhi         = persObj->m_dPhi         ;
  transObj->m_dEta         = persObj->m_dEta         ;
  transObj->m_cl_e_frac_S0 = persObj->m_cl_e_frac_S0 ;
  transObj->m_cl_e_frac_S1 = persObj->m_cl_e_frac_S1 ;
  transObj->m_cl_e_frac_S2 = persObj->m_cl_e_frac_S2 ;
  transObj->m_cl_e_frac_S3 = persObj->m_cl_e_frac_S3 ;
  transObj->m_Fside        = persObj->m_Fside ;
  transObj->m_Weta2        = persObj->m_Weta2 ;
  transObj->m_valid        = persObj->m_valid        ;

  m_eLinkEMClusterCnv.persToTrans( &persObj->m_cluster, &transObj->m_cluster, log ); 

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

  persObj->m_roiID        = transObj->m_roiID        ;
  persObj->m_HadEt        = transObj->m_HadEt        ;
  persObj->m_energyRatio  = transObj->m_energyRatio  ;
  persObj->m_rCore        = transObj->m_rCore        ;
  persObj->m_dPhi         = transObj->m_dPhi         ;
  persObj->m_dEta         = transObj->m_dEta         ;
  persObj->m_cl_e_frac_S0 = transObj->m_cl_e_frac_S0 ;
  persObj->m_cl_e_frac_S1 = transObj->m_cl_e_frac_S1 ;
  persObj->m_cl_e_frac_S2 = transObj->m_cl_e_frac_S2 ;
  persObj->m_cl_e_frac_S3 = transObj->m_cl_e_frac_S3 ;
  persObj->m_Fside        = transObj->m_Fside ;
  persObj->m_Weta2        = transObj->m_Weta2 ;
  persObj->m_valid        = transObj->m_valid        ;

  m_eLinkEMClusterCnv.transToPers( &transObj->m_cluster, &persObj->m_cluster, log );

  persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );  
    
}
