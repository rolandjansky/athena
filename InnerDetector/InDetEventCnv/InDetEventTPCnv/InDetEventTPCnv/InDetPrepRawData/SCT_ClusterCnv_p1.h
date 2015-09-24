/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTER_CNV_P1_H
#define SCT_CLUSTER_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SCT_Cluster.h"
#include "SCT_Cluster_p1.h"

#include "SiClusterCnv_p1.h"


class MsgStream;

class SCT_ClusterCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, InDet::SCT_Cluster, InDet::SCT_Cluster_p1>
{
public:
  SCT_ClusterCnv_p1() : m_siClusterCnv(0) {}

  void persToTrans( const InDet::SCT_Cluster_p1 *persObj, InDet::SCT_Cluster *transObj, MsgStream &log );
  void transToPers( const InDet::SCT_Cluster *transObj, InDet::SCT_Cluster_p1 *persObj,MsgStream &log );

private:
  SiClusterCnv_p1	*m_siClusterCnv;
};


#endif // SCT_CLUSTER_CNV_P1_H
