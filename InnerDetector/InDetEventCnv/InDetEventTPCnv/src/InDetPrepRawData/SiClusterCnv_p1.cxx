/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SiClusterCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SiCluster.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiClusterCnv_p1.h"


void SiClusterCnv_p1::
persToTrans( const InDet::SiCluster_p1 *persObj, InDet::SiCluster *transObj, MsgStream &log) 
{
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In SiCluster::persToTrans" << endreq;
  fillTransFromPStore( &m_rawDataCnv, persObj->m_rawData, transObj, log );
  fillTransFromPStore( &m_swCnv, persObj->m_width, &transObj->m_width, log );
}


void SiClusterCnv_p1::transToPers( const InDet::SiCluster *transObj, InDet::SiCluster_p1 *persObj, MsgStream &log )
{
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In SiCluster::transToPers" << endreq;
   persObj->m_rawData = baseToPersistent( &m_rawDataCnv, transObj, log );
   persObj->m_width = toPersistent( &m_swCnv, &transObj->width(), log );
}


