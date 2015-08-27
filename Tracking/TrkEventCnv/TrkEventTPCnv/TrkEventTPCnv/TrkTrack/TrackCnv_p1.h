/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_CNV_P1_H
#define TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TrakcCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkTrack/Track.h"
#include "TrkEventTPCnv/TrkTrack/Track_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p1.h"

class MsgStream;

class TrackCnv_p1: public T_AthenaPoolTPCnvBase<Trk :: Track,
                                                Trk :: Track_p1>
{
public:
  TrackCnv_p1() : m_fqCnv(0) {}

  virtual void persToTrans( const Trk::Track_p1 *, Trk::Track *, MsgStream& );
  virtual void transToPers( const Trk::Track *, Trk::Track_p1 *, MsgStream& );

  virtual void	initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
     m_trackStateVectorCnv.setTopConverter( topCnv, 0 );
  }
  
protected:
  typedef T_AthenaPoolTPPtrVectorCnv< DataVector<const Trk::TrackStateOnSurface>,
				      std::vector<TPObjRef>,
				      TrackStateOnSurfaceCnv_p1 >   TrackStateOSVectorCnv_p1;
  
  FitQualityCnv_p1          	*m_fqCnv;
  TrackStateOSVectorCnv_p1	m_trackStateVectorCnv;
};

#endif // TRACK_CNV_P1_H
