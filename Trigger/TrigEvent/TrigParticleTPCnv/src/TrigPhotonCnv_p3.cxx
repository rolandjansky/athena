/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigPhoton.h"
#include "TrigParticleTPCnv/TrigPhoton_p3.h"
#undef private
#undef protected
 
#include "TrigParticleTPCnv/TrigPhotonCnv_p3.h"
 
static ElementLinkCnv_p3< ElementLink<TrigEMClusterContainer> > m_eLinkEMClusterCnv;

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p3::persToTrans( const TrigPhoton_p3 *persObj,
				    TrigPhoton    *transObj,
				    MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p3::persToTrans called " << endreq;

  transObj->m_roiID        = persObj->m_roiID;

  transObj->m_HadEt        = persObj->m_allTheFloats[0] ;
  transObj->m_energyRatio  = persObj->m_allTheFloats[1] ;
  transObj->m_rCore        = persObj->m_allTheFloats[2] ;
  transObj->m_dPhi         = persObj->m_allTheFloats[3] ;
  transObj->m_dEta         = persObj->m_allTheFloats[4] ;
  transObj->m_cl_e_frac_S0 = persObj->m_allTheFloats[5] ;
  transObj->m_cl_e_frac_S1 = persObj->m_allTheFloats[6] ;
  transObj->m_cl_e_frac_S2 = persObj->m_allTheFloats[7] ;
  transObj->m_cl_e_frac_S3 = persObj->m_allTheFloats[8] ;
  transObj->m_Fside        = persObj->m_allTheFloats[9] ;
  transObj->m_Weta2        = persObj->m_allTheFloats[10];

  transObj->m_valid        = persObj->m_valid;

  m_eLinkEMClusterCnv.persToTrans( &persObj->m_cluster, &transObj->m_cluster, log ); 

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

  persObj->m_roiID        = transObj->m_roiID;

  persObj->m_allTheFloats[0] = transObj->m_HadEt        ;
  persObj->m_allTheFloats[1] = transObj->m_energyRatio  ;
  persObj->m_allTheFloats[2] = transObj->m_rCore        ;
  persObj->m_allTheFloats[3] = transObj->m_dPhi         ;
  persObj->m_allTheFloats[4] = transObj->m_dEta         ;
  persObj->m_allTheFloats[5] = transObj->m_cl_e_frac_S0 ;
  persObj->m_allTheFloats[6] = transObj->m_cl_e_frac_S1 ;
  persObj->m_allTheFloats[7] = transObj->m_cl_e_frac_S2 ;
  persObj->m_allTheFloats[8] = transObj->m_cl_e_frac_S3 ;
  persObj->m_allTheFloats[9] = transObj->m_Fside ;
  persObj->m_allTheFloats[10] = transObj->m_Weta2 ;

  persObj->m_valid        = transObj->m_valid;

  m_eLinkEMClusterCnv.transToPers( &transObj->m_cluster, &persObj->m_cluster, log );

  //persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );  
  m_p4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_P4PtEtaPhiM), log);  
}
