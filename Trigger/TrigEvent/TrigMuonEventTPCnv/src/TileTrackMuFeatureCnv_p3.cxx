/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p3.h"
#undef private
#undef protected
 
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p3.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TileTrackMuFeatureCnv_p3::persToTrans( const TileTrackMuFeature_p3 *persObj,
					     TileTrackMuFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TileTrackMuFeatureCnv_p3::persToTrans called " << endreq;

  transObj->m_PtTR_Trk          = persObj->m_allTheFloats[0];
  transObj->m_EtaTR_Trk         = persObj->m_allTheFloats[1];
  transObj->m_PhiTR_Trk         = persObj->m_allTheFloats[2];
  transObj->m_Typ_IDTrk         = persObj->m_Typ_IDTrk;

  m_ELink_TileMuCnv.persToTrans( &persObj->m_TileMu, &transObj->m_TileMuOutput, log);
  m_ELink_IDTkCnv.persToTrans(   &persObj->m_Track,  &transObj->m_IDScanOutput,  log);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TileTrackMuFeatureCnv_p3::transToPers( const TileTrackMuFeature    *transObj,
					     TileTrackMuFeature_p3 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TileTrackMuFeatureCnv_p3::transToPers called " << endreq;

  persObj->m_allTheFloats[0]   = transObj->m_PtTR_Trk;
  persObj->m_allTheFloats[1]   = transObj->m_EtaTR_Trk;
  persObj->m_allTheFloats[2]   = transObj->m_PhiTR_Trk;
  persObj->m_Typ_IDTrk  = transObj->m_Typ_IDTrk;

  m_ELink_TileMuCnv.transToPers( &transObj->m_TileMuOutput,  &persObj->m_TileMu, log);
  m_ELink_IDTkCnv.transToPers(   &transObj->m_IDScanOutput, &persObj->m_Track,  log);

}
