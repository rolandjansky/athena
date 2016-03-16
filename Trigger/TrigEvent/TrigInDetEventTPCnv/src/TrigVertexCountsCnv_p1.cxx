/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigVertexCounts.h"
#include "TrigInDetEventTPCnv/TrigVertexCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCountsCnv_p1.h"

void TrigVertexCountsCnv_p1::persToTrans( const TrigVertexCounts_p1 *persObj,
					TrigVertexCounts    *transObj,
					MsgStream& log ) {
  log << MSG::DEBUG << "TrigVertexCountsCnv_p1::persToTrans" << endreq;

  *transObj = TrigVertexCounts (persObj->m_vtxNtrks,
                                persObj->m_vtxTrkPtSqSum);
}

void TrigVertexCountsCnv_p1 :: transToPers( const TrigVertexCounts    *transObj,
					  TrigVertexCounts_p1 *persObj,
					  MsgStream& log ) {
  log << MSG::DEBUG << "TrigVertexCountsCnv_p1::transToPers" << endreq;
  
  persObj->m_vtxNtrks = transObj->vtxNtrks();
  persObj->m_vtxTrkPtSqSum = transObj->vtxTrkPtSqSum();
}
