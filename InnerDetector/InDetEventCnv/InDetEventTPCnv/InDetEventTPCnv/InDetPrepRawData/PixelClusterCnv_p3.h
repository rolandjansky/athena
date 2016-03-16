/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTER_CNV_P3_H
#define PIXELCLUSTER_CNV_P3_H

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelCluster_p3.h"

#include "SiWidthCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PixelID;

class PixelClusterCnv_p3
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, InDet::PixelCluster, InDet::PixelCluster_p3>
{
public:
  PixelClusterCnv_p3() {}
  PixelClusterCnv_p3( const PixelID * );

  InDet::PixelCluster
  createPixelCluster (const InDet::PixelCluster_p3* persObj,
                      const Identifier& waferId,
                      const InDetDD::SiDetectorElement* delEl,
                      MsgStream &log);

  void persToTrans( const InDet::PixelCluster_p3 *, InDet::PixelCluster *, MsgStream & );
  void transToPers( const InDet::PixelCluster *, InDet::PixelCluster_p3 *, MsgStream & );
        
protected:
    InDet::SiWidthCnv_p2  m_swCnv;
    LocalPositionCnv_p1	  m_localPosCnv;
    ErrorMatrixCnv_p1     m_errorMxCnv;
    const PixelID *       m_pixId2;
};

#endif // PIXELCLUSTER_CNV_P3_H
