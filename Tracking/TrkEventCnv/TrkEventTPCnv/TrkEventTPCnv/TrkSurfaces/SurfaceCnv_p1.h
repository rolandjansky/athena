/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SURFACE_CNV_P1_H
#define SURFACE_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/Surface.h"
#include "TrkEventTPCnv/TrkSurfaces/Surface_p1.h"

class MsgStream;

class SurfaceCnv_p1
   : public T_AthenaPoolTPAbstractPolyCnvBase<Trk::Surface, Trk::Surface, Trk::Surface_p1>
{
public:
  SurfaceCnv_p1() {}
        
  void persToTrans( const Trk::Surface_p1*, Trk::Surface*, MsgStream& );
  void transToPers( const Trk::Surface*, Trk::Surface_p1*, MsgStream& );
};

#endif // SURFACE_CNV_P1_H
