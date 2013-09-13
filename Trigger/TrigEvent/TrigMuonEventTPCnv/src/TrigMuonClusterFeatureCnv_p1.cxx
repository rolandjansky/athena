/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonClusterFeature.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_p1.h"
#undef private
#undef protected
 
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_p1.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigMuonClusterFeatureCnv_p1::persToTrans( const TrigMuonClusterFeature_p1 *persObj,
					     TrigMuonClusterFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigMuonClusterFeatureCnv_p1::persToTrans called " << endreq;

  transObj->m_eta    = persObj->m_eta    ;
  transObj->m_phi    = persObj->m_phi    ;
  transObj->m_nroi   = persObj->m_nroi   ;
  transObj->m_njet   = persObj->m_njet   ;
  transObj->m_ntrk   = persObj->m_ntrk   ;
     
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigMuonClusterFeatureCnv_p1::transToPers( const TrigMuonClusterFeature    *transObj,
					     TrigMuonClusterFeature_p1 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigMuonClusterFeatureCnv_p1::transToPers called " << endreq;

  persObj->m_eta    = transObj->m_eta    ;
  persObj->m_phi    = transObj->m_phi    ;
  persObj->m_nroi   = transObj->m_nroi   ;
  persObj->m_njet   = transObj->m_njet   ;
  persObj->m_ntrk   = transObj->m_ntrk   ;

    
}
