/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackCnv_p3.cxx
//
//-----------------------------------------------------------------------------
#include "TrkTrack/Track.h"
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p3.h"

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrackCnv_p3::persToTrans( const Trk::Track_p2 *persObj,
			       Trk::Track    *transObj,
			       MsgStream       &log )
{
    const Trk::TrackInfo * info = createTransFromPStore( &m_tiCnv, persObj->m_trackInfo, log );
    if(info )transObj->m_trackInfo  = *info;
    delete info;
    transObj->m_fitQuality.reset(
      createTransFromPStore(&m_fqCnv, persObj->m_fitQuality, log));
    std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>> sink(
      m_trackStateVectorCnv.createTransient(&persObj->m_trackState, log));
    // move copy
    transObj->m_trackStateVector = std::move(*sink);
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrackCnv_p3::transToPers( const Trk::Track    *,
			       Trk::Track_p2 *,
			       MsgStream       & )
{
  throw std::runtime_error("TrackCnv_p3::transToPers is deprecated!");   
}
