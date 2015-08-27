/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_STATE_ON_SURFACE_CNV_P1_H
#define TRACK_STATE_ON_SURFACE_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurface_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/ScatteringAngleOnTrackCnv_p1.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

class MsgStream;

class TrackStateOnSurfaceCnv_p1
   : public T_AthenaPoolTPCnvBase<Trk::TrackStateOnSurface, Trk::TrackStateOnSurface_p1>
{
public:
  TrackStateOnSurfaceCnv_p1() : m_scatCnv(0), m_fitQCnv(0), m_bremCnv(0), m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool")  {}
  
  virtual void persToTrans( const Trk::TrackStateOnSurface_p1 *persObj,
                Trk::TrackStateOnSurface    *transObj,
                MsgStream                     &log ) ;
  virtual void transToPers( const Trk::TrackStateOnSurface    *transObj,
                Trk::TrackStateOnSurface_p1 *persObj,
                MsgStream                      &log );
private:
  ScatteringAngleOnTrackCnv_p1             *m_scatCnv;
  FitQualityCnv_p1                         *m_fitQCnv;
  MaterialEffectsOnTrackCnv_p1             *m_bremCnv;
  ToolHandle<Trk::IEventCnvSuperTool>       m_eventCnvTool;
};

#endif // TRACK_STATE_ON_SURFACE_CNV_P1_H
