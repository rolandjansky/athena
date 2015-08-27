/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTANGLE_BOUNDS_CNV_P1_H
#define RECTANGLE_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RectangleBoundsCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/RectangleBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/RectangleBounds_p1.h"

class MsgStream;

class RectangleBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::RectangleBounds, Trk::RectangleBounds_p1 >
{
public:
  RectangleBoundsCnv_p1() {}
  
  void persToTrans( const Trk::RectangleBounds_p1*, Trk::RectangleBounds*, MsgStream& );
  void transToPers( const Trk::RectangleBounds*, Trk::RectangleBounds_p1*, MsgStream& ); 
};

#endif // RECTANGLE_BOUNDS_CNV_P1_H
