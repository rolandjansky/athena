/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p2.h"
#undef private
#undef protected
 
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p2.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TileTrackMuFeatureCnv_p2::persToTrans( const TileTrackMuFeature_p2 *persObj,
					     TileTrackMuFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TileTrackMuFeatureCnv_p2::persToTrans called " << endreq;

  transObj->m_PtTR_Trk          = persObj->m_PtTR_Trk;
  transObj->m_EtaTR_Trk         = persObj->m_EtaTR_Trk;
  transObj->m_PhiTR_Trk         = persObj->m_PhiTR_Trk;
  transObj->m_Typ_IDTrk         = persObj->m_Typ_IDTrk;

  m_ELink_TileMuCnv.persToTrans( &persObj->m_TileMu, &transObj->m_TileMuOutput, log);
  m_ELink_IDTkCnv.persToTrans(   &persObj->m_Track,  &transObj->m_IDScanOutput,  log);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TileTrackMuFeatureCnv_p2::transToPers( const TileTrackMuFeature    *transObj,
					     TileTrackMuFeature_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TileTrackMuFeatureCnv_p2::transToPers called " << endreq;

  persObj->m_PtTR_Trk   = transObj->m_PtTR_Trk;
  persObj->m_EtaTR_Trk  = transObj->m_EtaTR_Trk;
  persObj->m_PhiTR_Trk  = transObj->m_PhiTR_Trk;
  persObj->m_Typ_IDTrk  = transObj->m_Typ_IDTrk;

  m_ELink_TileMuCnv.transToPers( &transObj->m_TileMuOutput,  &persObj->m_TileMu, log);
  m_ELink_IDTkCnv.transToPers(   &transObj->m_IDScanOutput, &persObj->m_Track,  log);

}
