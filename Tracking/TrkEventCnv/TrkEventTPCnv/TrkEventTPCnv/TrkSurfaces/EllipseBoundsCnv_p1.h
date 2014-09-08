/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_ELLIPSEBOUNDS_P1_CNV_H
#define TRKEVENTTPCNV_ELLIPSEBOUNDS_P1_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/EllipseBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/EllipseBounds_p1.h"

class MsgStream;

class EllipseBoundsCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::SurfaceBounds, Trk::EllipseBounds, Trk::EllipseBounds_p1 >
{
public:
  EllipseBoundsCnv_p1() {}
  
  void persToTrans( const Trk::EllipseBounds_p1*, Trk::EllipseBounds*, MsgStream& );
  void transToPers( const Trk::EllipseBounds*, Trk::EllipseBounds_p1*, MsgStream& ); 
};

#endif 
