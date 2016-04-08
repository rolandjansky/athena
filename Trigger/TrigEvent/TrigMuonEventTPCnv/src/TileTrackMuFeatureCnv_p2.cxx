/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p2.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p2.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TileTrackMuFeatureCnv_p2::persToTrans( const TileTrackMuFeature_p2 *persObj,
					     TileTrackMuFeature    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TileTrackMuFeatureCnv_p2::persToTrans called " << endreq;

  ElementLink< TileMuFeatureContainer> tileMuOutput;
  m_ELink_TileMuCnv.persToTrans( &persObj->m_TileMu, &tileMuOutput, log);

  ElementLink< TrigInDetTrackCollection> IDScanOutput;
  m_ELink_IDTkCnv.persToTrans(   &persObj->m_Track,  &IDScanOutput,  log);

  *transObj = TileTrackMuFeature (persObj->m_PtTR_Trk,
                                  persObj->m_EtaTR_Trk,
                                  persObj->m_PhiTR_Trk,
                                  persObj->m_Typ_IDTrk,
                                  tileMuOutput,
                                  IDScanOutput);
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TileTrackMuFeatureCnv_p2::transToPers( const TileTrackMuFeature    *transObj,
					     TileTrackMuFeature_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TileTrackMuFeatureCnv_p2::transToPers called " << endreq;

  persObj->m_PtTR_Trk   = transObj->PtTR_Trk();
  persObj->m_EtaTR_Trk  = transObj->EtaTR_Trk();
  persObj->m_PhiTR_Trk  = transObj->PhiTR_Trk();
  persObj->m_Typ_IDTrk  = transObj->Typ_IDTrk();

  m_ELink_TileMuCnv.transToPers( &transObj->TileMuLink(),  &persObj->m_TileMu, log);
  m_ELink_IDTkCnv.transToPers(   &transObj->IDScanLink(), &persObj->m_Track,  log);
}
