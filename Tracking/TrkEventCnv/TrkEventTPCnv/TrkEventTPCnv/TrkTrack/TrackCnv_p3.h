/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_CNV_P3_H
#define TRACK_CNV_P3_H

//-----------------------------------------------------------------------------
//
// file:   TrakcCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkTrack/Track.h"
#include "TrkEventTPCnv/TrkTrack/Track_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p2.h"
#include "TrkEventTPCnv/TrkTrack/TrackInfoCnv_p1.h"
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummaryCnv_p2.h"

class MsgStream;

class TrackCnv_p3: public T_AthenaPoolTPCnvBase<Trk :: Track,
                                                Trk :: Track_p2>
{
public:
  TrackCnv_p3() : m_tiCnv(0),m_fqCnv(0),m_trackSummaryCnv(0) {}

  virtual void persToTrans( const Trk::Track_p2 *, Trk::Track *, MsgStream& );
  virtual void transToPers( const Trk::Track *, Trk::Track_p2 *, MsgStream& );

  virtual void	initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv )
  {
      // std::cout<<"initPrivateConverters for TrackCnv_p3="<<this<<" with topCnv="<<topCnv<<" and tscnv="<<m_trackSummaryCnv<<std::endl;
   m_trackStateVectorCnv.setTopConverter( topCnv, 0 );
   // m_trackSummaryCnv.setTopConverter( topCnv, 0);

  }
  
protected:
  typedef T_AthenaPoolTPPtrVectorCnv< DataVector<const Trk::TrackStateOnSurface>,
    std::vector<TPObjRef>,
    TrackStateOnSurfaceCnv_p2 >   TrackStateOSVectorCnv_p2;
    
  TrackInfoCnv_p1               *m_tiCnv;  
  FitQualityCnv_p1          	*m_fqCnv;
  TrackStateOSVectorCnv_p2	     m_trackStateVectorCnv;
  TrackSummaryCnv_p2*             m_trackSummaryCnv;
};

#endif // TRACK_CNV_P3_H
