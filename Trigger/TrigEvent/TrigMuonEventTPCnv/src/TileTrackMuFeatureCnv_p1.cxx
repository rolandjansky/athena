/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeature_p1.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p1.h"

void TileTrackMuFeatureCnv_p1::persToTrans(const TileTrackMuFeature_p1* persObj,
                                            TileTrackMuFeature* transObj,
                                            MsgStream &log)
{
   log << MSG::DEBUG << "TileTrackMuFeatureCnv_p1::persToTrans called " << endreq;
  transObj->m_PtTR_Trk          = persObj->m_PtTR_Trk;
  transObj->m_EtaTR_Trk         = persObj->m_EtaTR_Trk;
  transObj->m_PhiTR_Trk         = persObj->m_PhiTR_Trk;
  transObj->m_Typ_IDTrk         = persObj->m_Typ_IDTrk;

  //transObj->m_TileMuOutput      = createTransFromPStore( &m_TileMuCnv, persObj->m_TileMu, log);
  //transObj->m_IDScanOutput      = createTransFromPStore( &m_IDTkCnv,   persObj->m_Track,  log);

}

void TileTrackMuFeatureCnv_p1::transToPers(
		const TileTrackMuFeature* /*transObj*/,
		TileTrackMuFeature_p1* /*persObj*/,
                                            MsgStream &log)
{
   log << MSG::DEBUG << "TileTrackMuFeatureCnv_p1::transToPers called " << endreq;

  /*
  persObj->m_PtTR_Trk   = transObj->m_PtTR_Trk;
  persObj->m_EtaTR_Trk  = transObj->m_EtaTR_Trk;
  persObj->m_PhiTR_Trk  = transObj->m_PhiTR_Trk;
  persObj->m_Typ_IDTrk  = transObj->m_Typ_IDTrk;
 
  persObj->m_TileMu     = toPersistent( &m_TileMuCnv, transObj->m_TileMuOutput, log);
  persObj->m_Track      = toPersistent( &m_IDTkCnv,   transObj->m_IDScanOutput, log);
  */
}

