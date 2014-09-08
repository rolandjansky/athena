/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROTATEDTRAPEZOID_BOUNDS_CNV_P1_H
#define ROTATEDTRAPEZOID_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RotatedTrapezoidBoundsCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBounds_p1.h"

class MsgStream;

class RotatedTrapezoidBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::RotatedTrapezoidBounds, Trk::RotatedTrapezoidBounds_p1 >
{
public:
  RotatedTrapezoidBoundsCnv_p1() {}
  
  void persToTrans( const Trk::RotatedTrapezoidBounds_p1*, Trk::RotatedTrapezoidBounds*, MsgStream& );
  void transToPers( const Trk::RotatedTrapezoidBounds*, Trk::RotatedTrapezoidBounds_p1*, MsgStream& ); 
};

#endif // TRAPEZOID_BOUNDS_CNV_P1_H
