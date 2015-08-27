/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DISC_BOUNDS_CNV_P1_H
#define DISC_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   DiscBoundsCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/DiscBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/DiscBounds_p1.h"

class MsgStream;

class DiscBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::DiscBounds, Trk::DiscBounds_p1 >
{
public:
  DiscBoundsCnv_p1() {}
  
  void persToTrans( const Trk::DiscBounds_p1*, Trk::DiscBounds*, MsgStream& );
  void transToPers( const Trk::DiscBounds*, Trk::DiscBounds_p1*, MsgStream& ); 
};

#endif // DISC_BOUNDS_CNV_P1_H
