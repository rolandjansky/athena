/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTER_CNV_P2_H
#define PIXELCLUSTER_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/PixelCluster.h"
#include "PixelCluster_p2.h"

#include "SiWidthCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class PixelClusterCnv_p2
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, InDet::PixelCluster, InDet::PixelCluster_p2>
{
public:
  PixelClusterCnv_p2() {}

  InDet::PixelCluster
  createPixelCluster (const InDet::PixelCluster_p2* persObj,
                      const InDetDD::SiDetectorElement* delEl,
                      MsgStream &log);

  void persToTrans( const InDet::PixelCluster_p2 *, InDet::PixelCluster *, MsgStream & );
  void transToPers( const InDet::PixelCluster *, InDet::PixelCluster_p2 *, MsgStream & );
        
protected:
    InDet::SiWidthCnv_p2  m_swCnv;
    LocalPositionCnv_p1	m_localPosCnv;
    ErrorMatrixCnv_p1   	m_errorMxCnv;
};

#endif // PIXELCLUSTER_CNV_P2_H
