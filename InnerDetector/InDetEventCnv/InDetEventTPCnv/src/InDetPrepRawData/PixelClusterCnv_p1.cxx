/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p1.h"
#include "CxxUtils/make_unique.h"

void PixelClusterCnv_p1::
persToTrans( const InDet::PixelCluster_p1 *persObj, InDet::PixelCluster *transObj,MsgStream &log ) 
{
  Amg::Vector2D localPos;
  localPos.setZero();
  *transObj = 
    InDet::PixelCluster (Identifier(), // RDOId
                         localPos,
                         std::vector<Identifier>(), // rdoList
                         0, // lvl1a
                         0, // totalToT
                         std::vector<float>(), // chargeList
                         0, // totalCharge
                         InDet::SiWidth(),
                         nullptr, //detEl
                         nullptr, // cmat
                         persObj->m_omegax,
                         persObj->m_omegay,
                         0 //splitInfo
                         );
  fillTransFromPStore( &m_siClusterCnv, persObj->m_siCluster, transObj, log );
}

void PixelClusterCnv_p1::
transToPers( const InDet::PixelCluster *transObj, InDet::PixelCluster_p1 *persObj, MsgStream &log )
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "PixelClusterCnv_p1::transToPers" << endreq;
   persObj->m_siCluster = baseToPersistent( &m_siClusterCnv, transObj, log );
   persObj->m_omegax = transObj->omegax();
   persObj->m_omegay = transObj->omegay();
}


