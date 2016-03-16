/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkEventTPCnv/VxVertex/VxCandidateCnv_p2.h"
#include <iostream>

void  VxCandidateCnv_p2::persToTrans(const Trk::VxCandidate_p1 * persObj, Trk::VxCandidate * transObj, MsgStream &log) {
    
  Trk::RecVertex vx;
  fillTransFromPStore(&m_recVertexConverter, persObj->m_recVertex, &vx, log);
    
  std::vector<Trk::VxTrackAtVertex*> trackAtVertex;
  if (persObj->m_vxTrackAtVertex.size()){ // not to call if 0
    trackAtVertex.reserve( persObj->m_vxTrackAtVertex.size() );
    for( std::vector<TPObjRef>::const_iterator it = persObj->m_vxTrackAtVertex.begin(); it != persObj->m_vxTrackAtVertex.end();  ++it ) 
      trackAtVertex.push_back( createTransFromPStore( &m_vxTrackAtVertexConverter, *it, log ) );    
  }
    
  *transObj = Trk::VxCandidate (std::move(vx),
                                std::move (trackAtVertex));
  transObj->setVertexType (static_cast<Trk::VertexType>(persObj->m_vertexType));
    
//    std::cout<<"m_recVertex:"<<transObj->m_recVertex<<std::endl;
     
//    std::vector<Trk::VxTrackAtVertex*>::iterator it=transObj->m_vxTrackAtVertex.begin();
//    for (;it!=transObj->m_vxTrackAtVertex.end();++it)
//        std::cout<<*(*it)<<std::endl;
 
}

void  VxCandidateCnv_p2::transToPers(const Trk::VxCandidate * transObj, Trk::VxCandidate_p1 * persObj, MsgStream &log)
{
    
     // std::cout<<"VxCandidateCnv_p2 transToPers"<<std::endl;
  persObj->m_recVertex = toPersistent( &m_recVertexConverter, &(transObj->recVertex()), log );
 
 // get rid of the refitted perigees in case of primary and pileup vertex
  const std::vector<Trk::VxTrackAtVertex*>& trackAtVertex = *transObj->vxTrackAtVertex();
  if (transObj->vertexType() == Trk::PriVtx || transObj->vertexType() == Trk::PileUp) {
   for (Trk::VxTrackAtVertex* t : trackAtVertex) {
     t->setPerigeeAtVertex (static_cast<Trk::TrackParameters*>(nullptr));
   }
 }
 m_vxTrkAtVrtCnv.transToPers( &trackAtVertex, &persObj->m_vxTrackAtVertex, log);
 persObj->m_vertexType = static_cast<int>(transObj->vertexType());
}
