/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p1.h"

void SCT_ClusterCnv_p1::
persToTrans( const InDet::SCT_Cluster_p1 *persObj, InDet::SCT_Cluster *transObj,MsgStream &log ) 
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "SCT_ClusterCnv_p1::persToTrans" << endreq;
   fillTransFromPStore( &m_siClusterCnv, persObj->m_siCluster, transObj, log );
}

void SCT_ClusterCnv_p1::
transToPers( const InDet::SCT_Cluster *transObj, InDet::SCT_Cluster_p1 *persObj, MsgStream &log )
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "SCT_ClusterCnv_p1::transToPers" << endreq;
   persObj->m_siCluster = baseToPersistent( &m_siClusterCnv, transObj, log );
}


