/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIANGLE_BOUNDS_CNV_P1_H
#define TRIANGLE_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TriangleBounds_p1.h
//
//-----------------------------------------------------------------------------

#include "TrkSurfaces/TriangleBounds.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventTPCnv/TrkSurfaces/TriangleBounds_p1.h"

class MsgStream;

// namespace Trk {
//     class TriangleBounds;
//     class TriangleBounds_p1;
//     class SurfaceBounds;
// }

class TriangleBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::TriangleBounds, Trk::TriangleBounds_p1 >
{
public:
    TriangleBoundsCnv_p1() {};
  
  void persToTrans( const Trk::TriangleBounds_p1*, Trk::TriangleBounds*, MsgStream& );
  void transToPers( const Trk::TriangleBounds*, Trk::TriangleBounds_p1*, MsgStream& ); 
};

#endif
