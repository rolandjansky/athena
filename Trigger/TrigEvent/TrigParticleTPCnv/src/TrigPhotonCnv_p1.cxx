/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigPhoton.h"
#include "TrigParticleTPCnv/TrigPhoton_p1.h"
#undef private
#undef protected
 
#include "TrigParticleTPCnv/TrigPhotonCnv_p1.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p1::persToTrans( const TrigPhoton_p1 *persObj,
					     TrigPhoton    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p1::persToTrans called " << endreq;

  transObj->m_roiID        = persObj->m_roiID        ;
  transObj->m_HadEt        = persObj->m_HadEt        ;
  transObj->m_energyRatio  = persObj->m_energyRatio  ;
  transObj->m_rCore        = persObj->m_rCore        ;
  transObj->m_dPhi         = persObj->m_dPhi         ;
  transObj->m_dEta         = persObj->m_dEta         ;
  transObj->m_valid        = persObj->m_valid        ;
 
  transObj->m_cluster.reset();
  log << MSG::WARNING << "TrigPhoton->m_cluster being set to NULL because you are trying to read TrigPhoton_p1" << endreq;

  fillTransFromPStore( &m_P4PtEtaPhiMCnv, persObj->m_P4PtEtaPhiM, transObj, log );     
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p1::transToPers( const TrigPhoton*,
					     TrigPhoton_p1*,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p1::transToPers called " << endreq;
  log << MSG::WARNING << "TrigPhotonCnv_p1::transToPers should not be called anymore! " << endreq;
    
}
