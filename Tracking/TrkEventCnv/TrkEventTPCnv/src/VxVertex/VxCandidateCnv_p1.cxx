/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   VxCandidate_p1.cxx
//
//-----------------------------------------------------------------------------

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkEventTPCnv/VxVertex/VxCandidateCnv_p1.h"
#include <iostream>
void  VxCandidateCnv_p1::persToTrans(const Trk::VxCandidate_p1 * persObj, Trk::VxCandidate * transObj, MsgStream &log) {

  Trk::RecVertex vx;
  fillTransFromPStore(&m_recVertexConverter, persObj->m_recVertex, &vx, log);
  std::vector<Trk::VxTrackAtVertex*> trackAtVertex;
  m_vxTrkAtVrtCnv.persToTrans(&persObj->m_vxTrackAtVertex, &trackAtVertex, log); 

  *transObj = Trk::VxCandidate (std::move(vx),
                                std::move (trackAtVertex));
  transObj->setVertexType (static_cast<Trk::VertexType>(persObj->m_vertexType));
}

void  VxCandidateCnv_p1::transToPers(const Trk::VxCandidate * transObj, Trk::VxCandidate_p1 * persObj, MsgStream &log)
{
  persObj->m_recVertex = toPersistent( &m_recVertexConverter, &(transObj->recVertex()), log );
 
 // get rid of the refitted perigees in case of primary and pileup vertex

  const std::vector<Trk::VxTrackAtVertex*>& trackAtVertex = *transObj->vxTrackAtVertex();
  if (transObj->vertexType() == Trk::PriVtx || transObj->vertexType() == Trk::PileUp)
 {
   for (Trk::VxTrackAtVertex* t : trackAtVertex) {
     t->setPerigeeAtVertex (static_cast<Trk::TrackParameters*>(nullptr));
   }
 }
 m_vxTrkAtVrtCnv.transToPers( &trackAtVertex, &persObj->m_vxTrackAtVertex, log);
 persObj->m_vertexType = static_cast<int>(transObj->vertexType());
}
