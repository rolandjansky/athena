/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_CLUSTER_CNV_P1_H
#define PIXEL_CLUSTER_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   PixelClusterCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/PixelCluster.h"
#include "PixelCluster_p1.h"

#include "SiClusterCnv_p1.h"

class MsgStream;

class PixelClusterCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, InDet::PixelCluster, InDet::PixelCluster_p1 >
{
    public:
  PixelClusterCnv_p1() : m_siClusterCnv(0) {}

  void persToTrans( const InDet::PixelCluster_p1 *persObj,
		    InDet::PixelCluster    *transObj,
		    MsgStream                &log );
  void transToPers( const InDet::PixelCluster    *transObj,
		    InDet::PixelCluster_p1 *persObj,
		    MsgStream                &log );

protected:        
  SiClusterCnv_p1	    *m_siClusterCnv;
};

#endif // PIXEL_CLUSTER_CNV_P1_H
