/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p2.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void IsoMuonFeatureCnv_p2::persToTrans( const IsoMuonFeature_p2 *persObj,
					     IsoMuonFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "IsoMuonFeatureCnv_p2::persToTrans called " << endreq;

  transObj->m_EtInnerConeEC    = persObj->m_allTheFloats[0]    ;
  transObj->m_EtOuterConeEC    = persObj->m_allTheFloats[1]    ;
  transObj->m_EtInnerConeHC    = persObj->m_allTheFloats[2]    ;
  transObj->m_EtOuterConeHC    = persObj->m_allTheFloats[3]    ;

  transObj->m_NTracksCone      = persObj->m_NTracksCone        ;

  transObj->m_SumPtTracksCone  = persObj->m_allTheFloats[4]    ;
  transObj->m_PtMuTracksCone   = persObj->m_allTheFloats[5]    ;
  transObj->m_LAr_w            = persObj->m_allTheFloats[6]    ;
  transObj->m_Tile_w           = persObj->m_allTheFloats[7]    ;

  transObj->m_RoiIdMu          = persObj->m_RoiIdMu            ;

  transObj->m_PtMu             = persObj->m_allTheFloats[8]    ;
  transObj->m_EtaMu            = persObj->m_allTheFloats[9]    ;
  transObj->m_PhiMu            = persObj->m_allTheFloats[10]   ;
     
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void IsoMuonFeatureCnv_p2::transToPers( const IsoMuonFeature    *transObj,
					     IsoMuonFeature_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "IsoMuonFeatureCnv_p1::transToPers called " << endreq;

  persObj->m_allTheFloats[0]  = transObj->m_EtInnerConeEC    ;
  persObj->m_allTheFloats[1]  = transObj->m_EtOuterConeEC    ;
  persObj->m_allTheFloats[2]  = transObj->m_EtInnerConeHC    ;
  persObj->m_allTheFloats[3]  = transObj->m_EtOuterConeHC    ;

  persObj->m_NTracksCone      = transObj->m_NTracksCone      ;

  persObj->m_allTheFloats[4]  = transObj->m_SumPtTracksCone  ;
  persObj->m_allTheFloats[5]  = transObj->m_PtMuTracksCone   ;
  persObj->m_allTheFloats[6]  = transObj->m_LAr_w            ;
  persObj->m_allTheFloats[7]  = transObj->m_Tile_w           ;

  persObj->m_RoiIdMu          = transObj->m_RoiIdMu          ;

  persObj->m_allTheFloats[8]  = transObj->m_PtMu             ;
  persObj->m_allTheFloats[9]  = transObj->m_EtaMu            ;
  persObj->m_allTheFloats[10] = transObj->m_PhiMu            ;

    
}
