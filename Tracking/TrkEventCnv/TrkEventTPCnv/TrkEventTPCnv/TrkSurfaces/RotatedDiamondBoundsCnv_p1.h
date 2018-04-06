/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROTATED_DIAMOND_BOUNDS_CNV_P1_H
#define ROTATED_DIAMOND_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RotatedDiamondBoundsCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/RotatedDiamondBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/RotatedDiamondBounds_p1.h"

class MsgStream;

class RotatedDiamondBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::RotatedDiamondBounds, Trk::RotatedDiamondBounds_p1 >
{
public:
  RotatedDiamondBoundsCnv_p1() {}
  
  void persToTrans( const Trk::RotatedDiamondBounds_p1*, Trk::RotatedDiamondBounds*, MsgStream& );
  void transToPers( const Trk::RotatedDiamondBounds*, Trk::RotatedDiamondBounds_p1*, MsgStream& ); 
};

#endif // ROTATED_DIAMOND_BOUNDS_CNV_P1_H

