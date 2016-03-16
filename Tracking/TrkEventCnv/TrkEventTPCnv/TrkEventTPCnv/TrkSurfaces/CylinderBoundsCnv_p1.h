/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CYLINDER_BOUNDS_CNV_P1_H
#define CYLINDER_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CylinderBoundsCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/CylinderBounds.h"

#include "TrkEventTPCnv/TrkSurfaces/CylinderBounds_p1.h"

class MsgStream;

class CylinderBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::CylinderBounds, Trk::CylinderBounds_p1 >
{
public:
  CylinderBoundsCnv_p1() {}
  
  void persToTrans( const Trk::CylinderBounds_p1*, Trk::CylinderBounds*, MsgStream& );
  void transToPers( const Trk::CylinderBounds*, Trk::CylinderBounds_p1*, MsgStream& ); 
};

#endif // CYLINDER_BOUNDS_CNV_P1_H
