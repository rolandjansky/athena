/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#undef private
#undef protected
#include "TrkEventTPCnv/VxVertex/VxCandidateCnv_p2.h"
#include <iostream>

void  VxCandidateCnv_p2::persToTrans(const Trk::VxCandidate_p1 * persObj, Trk::VxCandidate * transObj, MsgStream &log) {
    
    fillTransFromPStore(&m_recVertexConverter, persObj->m_recVertex, &transObj->m_recVertex, log);
    
    if (persObj->m_vxTrackAtVertex.size()){ // not to call if 0
        transObj->m_vxTrackAtVertex.reserve( persObj->m_vxTrackAtVertex.size() );
        for( std::vector<TPObjRef>::const_iterator it = persObj->m_vxTrackAtVertex.begin(); it != persObj->m_vxTrackAtVertex.end();  ++it ) 
            transObj->m_vxTrackAtVertex.push_back( createTransFromPStore( &m_vxTrackAtVertexConverter, *it, log ) );    
    }
    
    transObj->m_vertexType = static_cast<Trk::VertexType>(persObj->m_vertexType);
    
//    std::cout<<"m_recVertex:"<<transObj->m_recVertex<<std::endl;
     
//    std::vector<Trk::VxTrackAtVertex*>::iterator it=transObj->m_vxTrackAtVertex.begin();
//    for (;it!=transObj->m_vxTrackAtVertex.end();++it)
//        std::cout<<*(*it)<<std::endl;
 
}

void  VxCandidateCnv_p2::transToPers(const Trk::VxCandidate * transObj, Trk::VxCandidate_p1 * persObj, MsgStream &log)
{
    
     // std::cout<<"VxCandidateCnv_p2 transToPers"<<std::endl;
 persObj->m_recVertex = toPersistent( &m_recVertexConverter, &(transObj->m_recVertex), log );
 
 // get rid of the refitted perigees in case of primary and pileup vertex
 if (transObj->m_vertexType == Trk::PriVtx || transObj->m_vertexType == Trk::PileUp) {
   for (std::vector<Trk::VxTrackAtVertex*>::const_iterator itr  = transObj->m_vxTrackAtVertex.begin(); itr != transObj->m_vxTrackAtVertex.end()  ; itr++)
   {
     Trk::VxTrackAtVertex* tmp = (*itr);
     delete tmp->m_perigeeAtVertex;
     tmp->m_perigeeAtVertex = 0;
   }
 }
 m_vxTrkAtVrtCnv.transToPers( &transObj->m_vxTrackAtVertex, &persObj->m_vxTrackAtVertex, log);
 persObj->m_vertexType = static_cast<int>(transObj->m_vertexType);
}
