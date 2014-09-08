/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRAPEZOID_BOUNDS_CNV_P1_H
#define TRAPEZOID_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TrapezoidBoundsCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBounds_p1.h"

class MsgStream;

class TrapezoidBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::TrapezoidBounds, Trk::TrapezoidBounds_p1 >
{
public:
  TrapezoidBoundsCnv_p1() {}
  
  void persToTrans( const Trk::TrapezoidBounds_p1*, Trk::TrapezoidBounds*, MsgStream& );
  void transToPers( const Trk::TrapezoidBounds*, Trk::TrapezoidBounds_p1*, MsgStream& ); 
};

#endif // TRAPEZOID_BOUNDS_CNV_P1_H
