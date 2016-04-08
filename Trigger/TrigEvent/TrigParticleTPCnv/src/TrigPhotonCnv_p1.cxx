/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigPhoton.h"
#include "TrigParticleTPCnv/TrigPhoton_p1.h"
#include "TrigParticleTPCnv/TrigPhotonCnv_p1.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigPhotonCnv_p1::persToTrans( const TrigPhoton_p1 *persObj,
					     TrigPhoton    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigPhotonCnv_p1::persToTrans called " << endreq;

  *transObj = TrigPhoton (0, 0, 0,
                          persObj->m_roiID,
                          persObj->m_valid,
                          ElementLink< TrigEMClusterContainer >(),
                          persObj->m_HadEt,
                          persObj->m_energyRatio,
                          persObj->m_rCore,
                          persObj->m_dEta,
                          persObj->m_dPhi,
                          0, // Fside
                          0, // Weta2
                          0, // F0
                          0, // F1
                          0, // F2
                          0  // F3
                          );

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
