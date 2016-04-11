/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLANAR_CLUSTER_CNV_P2_H
#define PLANAR_CLUSTER_CNV_P2_H

//--------------------------------------------------------------
//
// file:    PlanarClusterCnv_p2.h
//
//--------------------------------------------------------------

#include "ISF_FatrasEvent/PlanarCluster.h"
#include "PlanarCluster_p2.h"

#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PixelID;
class SCT_ID;

class PlanarClusterCnv_p2 : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, iFatras::PlanarCluster, iFatras::PlanarCluster_p2 > 
{
 public:
  PlanarClusterCnv_p2() {}
  PlanarClusterCnv_p2( const PixelID *, const SCT_ID * );

  iFatras::PlanarCluster
  createPlanarCluster (const iFatras::PlanarCluster_p2 *persObj,
                       const Identifier& waferId,
                       const iFatras::PlanarDetElement* detEl,
                       MsgStream &log);

  void persToTrans( const iFatras::PlanarCluster_p2 *persObj,
		    iFatras::PlanarCluster *transobj,
		    MsgStream &log);
  void transToPers( const iFatras::PlanarCluster *transobj,
		    iFatras::PlanarCluster_p2 *persObj,
		    MsgStream &log);

 protected:
  InDet::SiWidthCnv_p2  m_swCnv;
  LocalPositionCnv_p1   m_localPosCnv;
  ErrorMatrixCnv_p1     m_errorMxCnv;
  const PixelID *       m_pixId2;
  const SCT_ID  *       m_sctId2;
};

#endif // PLANAR_CLUSTER_CNV_P2_H
