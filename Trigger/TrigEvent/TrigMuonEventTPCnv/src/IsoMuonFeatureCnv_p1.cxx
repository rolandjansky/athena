/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p1.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void IsoMuonFeatureCnv_p1::persToTrans( const IsoMuonFeature_p1 *persObj,
					     IsoMuonFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "IsoMuonFeatureCnv_p1::persToTrans called " << endreq;

  transObj->m_EtInnerConeEC    = persObj->m_EtInnerConeEC    ;
  transObj->m_EtOuterConeEC    = persObj->m_EtOuterConeEC    ;
  transObj->m_EtInnerConeHC    = persObj->m_EtInnerConeHC    ;
  transObj->m_EtOuterConeHC    = persObj->m_EtOuterConeHC    ;
  transObj->m_NTracksCone      = persObj->m_NTracksCone      ;
  transObj->m_SumPtTracksCone  = persObj->m_SumPtTracksCone  ;
  transObj->m_PtMuTracksCone   = persObj->m_PtMuTracksCone   ;
  transObj->m_LAr_w            = persObj->m_LAr_w            ;
  transObj->m_Tile_w           = persObj->m_Tile_w           ;
  transObj->m_RoiIdMu          = persObj->m_RoiIdMu          ;
  transObj->m_PtMu             = persObj->m_PtMu             ;
  transObj->m_EtaMu            = persObj->m_EtaMu            ;
  transObj->m_PhiMu            = persObj->m_PhiMu            ;
     
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void IsoMuonFeatureCnv_p1::transToPers( const IsoMuonFeature    *transObj,
					     IsoMuonFeature_p1 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "IsoMuonFeatureCnv_p1::transToPers called " << endreq;

  persObj->m_EtInnerConeEC    = transObj->m_EtInnerConeEC    ;
  persObj->m_EtOuterConeEC    = transObj->m_EtOuterConeEC    ;
  persObj->m_EtInnerConeHC    = transObj->m_EtInnerConeHC    ;
  persObj->m_EtOuterConeHC    = transObj->m_EtOuterConeHC    ;
  persObj->m_NTracksCone      = transObj->m_NTracksCone      ;
  persObj->m_SumPtTracksCone  = transObj->m_SumPtTracksCone  ;
  persObj->m_PtMuTracksCone   = transObj->m_PtMuTracksCone   ;
  persObj->m_LAr_w            = transObj->m_LAr_w            ;
  persObj->m_Tile_w           = transObj->m_Tile_w           ;
  persObj->m_RoiIdMu          = transObj->m_RoiIdMu          ;
  persObj->m_PtMu             = transObj->m_PtMu             ;
  persObj->m_EtaMu            = transObj->m_EtaMu            ;
  persObj->m_PhiMu            = transObj->m_PhiMu            ;

    
}
