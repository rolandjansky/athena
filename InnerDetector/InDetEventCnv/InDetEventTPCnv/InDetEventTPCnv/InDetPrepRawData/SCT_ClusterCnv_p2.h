/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTER_CNV_P2_H
#define SCT_CLUSTER_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SCT_Cluster.h"
#include "SCT_Cluster_p2.h"

#include "SiWidthCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetIdentifier/SCT_ID.h"

class MsgStream;
//class SCT_ID;

class SCT_ClusterCnv_p2
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, InDet::SCT_Cluster, InDet::SCT_Cluster_p2>
{
public:
  SCT_ClusterCnv_p2() {};
  SCT_ClusterCnv_p2( const SCT_ID * );

  InDet::SCT_Cluster
  createSCT_Cluster (const InDet::SCT_Cluster_p2* persObj,
                     Identifier clusId,
                     const InDetDD::SiDetectorElement* detEl,
                     MsgStream& log);

  void persToTrans( const InDet::SCT_Cluster_p2 *, InDet::SCT_Cluster *, MsgStream & );
  void transToPers( const InDet::SCT_Cluster *, InDet::SCT_Cluster_p2 *, MsgStream & );
        
protected:
  const SCT_ID *m_sctId2;
  InDet::SiWidthCnv_p2  m_swCnv;
  LocalPositionCnv_p1	m_localPosCnv;
  ErrorMatrixCnv_p1   	m_errorMxCnv;
};

#endif // SCT_CLUSTER_CNV_P2_H
