/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DIAMOND_BOUNDS_CNV_P1_H
#define DIAMOND_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   DiamondBoundsCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/DiamondBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/DiamondBounds_p1.h"

class MsgStream;

class DiamondBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::DiamondBounds, Trk::DiamondBounds_p1 >
{
public:
  DiamondBoundsCnv_p1() {}
  
  void persToTrans( const Trk::DiamondBounds_p1*, Trk::DiamondBounds*, MsgStream& );
  void transToPers( const Trk::DiamondBounds*, Trk::DiamondBounds_p1*, MsgStream& ); 
};

#endif // DIAMOND_BOUNDS_CNV_P1_H

