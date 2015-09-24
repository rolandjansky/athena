/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetPrepRawData/PixelCluster.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p1.h"

void PixelClusterCnv_p1::
persToTrans( const InDet::PixelCluster_p1 *persObj, InDet::PixelCluster *transObj,MsgStream &log ) 
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "PixelClusterCnv_p1::persToTrans" << endreq;
   fillTransFromPStore( &m_siClusterCnv, persObj->m_siCluster, transObj, log );
   transObj->m_omegax = persObj->m_omegax;
   transObj->m_omegay = persObj->m_omegay;
}

void PixelClusterCnv_p1::
transToPers( const InDet::PixelCluster *transObj, InDet::PixelCluster_p1 *persObj, MsgStream &log )
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "PixelClusterCnv_p1::transToPers" << endreq;
   persObj->m_siCluster = baseToPersistent( &m_siClusterCnv, transObj, log );
   persObj->m_omegax = transObj->m_omegax;
   persObj->m_omegay = transObj->m_omegay;
}


