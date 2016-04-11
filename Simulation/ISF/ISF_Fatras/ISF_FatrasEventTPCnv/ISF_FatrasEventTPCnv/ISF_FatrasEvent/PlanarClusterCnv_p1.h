/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLANAR_CLUSTER_CNV_P1_H
#define PLANAR_CLUSTER_CNV_P1_H

//--------------------------------------------------------------
//
// file:    PlanarClusterCnv_p1.h
//
//--------------------------------------------------------------

#include "ISF_FatrasEvent/PlanarCluster.h"
#include "PlanarCluster_p1.h"

#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PixelID;

class PlanarClusterCnv_p1 : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, iFatras::PlanarCluster, iFatras::PlanarCluster_p1 > 
{
 public:
  PlanarClusterCnv_p1() {}
  PlanarClusterCnv_p1( const PixelID * );

  iFatras::PlanarCluster
  createPlanarCluster (const iFatras::PlanarCluster_p1 *persObj,
                       const Identifier& waferId,
                       const iFatras::PlanarDetElement* detEl,
                       MsgStream &log);

  void persToTrans( const iFatras::PlanarCluster_p1 *persObj,
		    iFatras::PlanarCluster *transobj,
		    MsgStream &log);
  void transToPers( const iFatras::PlanarCluster *transobj,
		    iFatras::PlanarCluster_p1 *persObj,
		    MsgStream &log);

 protected:
  InDet::SiWidthCnv_p2  m_swCnv;
  LocalPositionCnv_p1   m_localPosCnv;
  ErrorMatrixCnv_p1     m_errorMxCnv;
  const PixelID *       m_pixId2;
};

#endif // PLANAR_CLUSTER_CNV_P1_H
