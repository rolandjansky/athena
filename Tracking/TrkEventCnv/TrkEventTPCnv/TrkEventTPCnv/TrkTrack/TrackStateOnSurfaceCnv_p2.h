/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_STATE_ON_SURFACE_CNV_P2_H
#define TRACK_STATE_ON_SURFACE_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurface_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
//#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBaseCnv_p1.h"

class MsgStream;

class TrackStateOnSurfaceCnv_p2
   : public T_AthenaPoolTPCnvBase<Trk::TrackStateOnSurface, Trk::TrackStateOnSurface_p2>
{
public:
  TrackStateOnSurfaceCnv_p2() : m_fitQCnv(0) {}
  
  virtual void persToTrans( const Trk::TrackStateOnSurface_p2 *persObj,
			    Trk::TrackStateOnSurface    *transObj,
			    MsgStream                     &log ) ;
  virtual void transToPers( const Trk::TrackStateOnSurface    *transObj,
			    Trk::TrackStateOnSurface_p2 *persObj,
			    MsgStream                      &log );
private:
  FitQualityCnv_p1               	*m_fitQCnv;
  //  MaterialEffectsBaseCnv_p1         *m_matBaseCnv;
};

#endif // TRACK_STATE_ON_SURFACE_CNV_P2_H
