/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p3.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void IsoMuonFeatureCnv_p3::persToTrans( const IsoMuonFeature_p3 *persObj,
					     IsoMuonFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "IsoMuonFeatureCnv_p3::persToTrans called " << endreq;

  transObj->m_sumet01    = persObj->m_allTheFloats[0]    ;
  transObj->m_sumet02    = persObj->m_allTheFloats[1]    ;
  transObj->m_sumet03    = persObj->m_allTheFloats[2]    ;
  transObj->m_sumet04    = persObj->m_allTheFloats[3]    ;
  transObj->m_sumpt01    = persObj->m_allTheFloats[4]    ;
  transObj->m_sumpt02    = persObj->m_allTheFloats[5]    ;
  transObj->m_sumpt03    = persObj->m_allTheFloats[6]    ;
  transObj->m_sumpt04    = persObj->m_allTheFloats[7]    ;
  transObj->m_PtMuID     = persObj->m_allTheFloats[8]    ;
  transObj->m_MaxPtID    = persObj->m_allTheFloats[9]    ;
  transObj->m_flag       = persObj->m_flag               ;

  transObj->m_RoiIdMu    = persObj->m_RoiIdMu            ;
  transObj->m_PtMu       = persObj->m_allTheFloats[10]   ;
  transObj->m_QMu        = persObj->m_allTheFloats[11]   ;
  transObj->m_EtaMu      = persObj->m_allTheFloats[12]   ;
  transObj->m_PhiMu      = persObj->m_allTheFloats[13]   ;
     
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void IsoMuonFeatureCnv_p3::transToPers( const IsoMuonFeature    *transObj,
					     IsoMuonFeature_p3 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "IsoMuonFeatureCnv_p3::transToPers called " << endreq;

  persObj->m_allTheFloats[0]  = transObj->m_sumet01 ;
  persObj->m_allTheFloats[1]  = transObj->m_sumet02 ;
  persObj->m_allTheFloats[2]  = transObj->m_sumet03 ;
  persObj->m_allTheFloats[3]  = transObj->m_sumet04 ;
  persObj->m_allTheFloats[4]  = transObj->m_sumpt01 ;
  persObj->m_allTheFloats[5]  = transObj->m_sumpt02 ;
  persObj->m_allTheFloats[6]  = transObj->m_sumpt03 ;
  persObj->m_allTheFloats[7]  = transObj->m_sumpt04 ;
  persObj->m_allTheFloats[8]  = transObj->m_PtMuID  ;
  persObj->m_allTheFloats[9]  = transObj->m_MaxPtID ;

  persObj->m_flag             = transObj->m_flag    ;
  persObj->m_RoiIdMu          = transObj->m_RoiIdMu ;

  persObj->m_allTheFloats[10] = transObj->m_PtMu    ;
  persObj->m_allTheFloats[11] = transObj->m_QMu     ;
  persObj->m_allTheFloats[12] = transObj->m_EtaMu   ;
  persObj->m_allTheFloats[13] = transObj->m_PhiMu   ;
    
}
