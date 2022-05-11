/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONE_BOUNDS_CNV_P1_H
#define CONE_BOUNDS_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   ConeBoundsCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/ConeBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/ConeBounds_p1.h"

class MsgStream;

class ConeBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::ConeBounds, Trk::ConeBounds_p1 >
{
public:
  ConeBoundsCnv_p1() {}
  
  void persToTrans( const Trk::ConeBounds_p1*, Trk::ConeBounds*, MsgStream& );
  void transToPers( const Trk::ConeBounds*, Trk::ConeBounds_p1*, MsgStream& ); 
};

#endif // DISC_BOUNDS_CNV_P1_H
