/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_CNV_P4_H
#define TRACK_CNV_P4_H

//-----------------------------------------------------------------------------
//
// file:   TrakcCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkTrack/Track.h"
#include "TrkEventTPCnv/TrkTrack/Track_p4.h"

#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p3.h"

class MsgStream;

class TrackCnv_p4: public T_AthenaPoolTPCnvBase<Trk :: Track,
                                                Trk :: Track_p4>
{
public:
  TrackCnv_p4() {}
  ~TrackCnv_p4(){}

  virtual void persToTrans( const Trk::Track_p4 *, Trk::Track *, MsgStream& );
  virtual void transToPers( const Trk::Track *, Trk::Track_p4 *, MsgStream& );

  virtual void  initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv )
  {
      // std::cout<<"initPrivateConverters for TrackCnv_p4="<<this<<" with topCnv="<<topCnv<<" and tscnv="<<m_trackSummaryCnv<<std::endl;
   m_trackStateVectorCnv.setTopConverter( topCnv, 0 );
   // m_trackSummaryCnv.setTopConverter( topCnv, 0);

  }
  
protected:
  typedef T_AthenaPoolTPPtrVectorCnv< DataVector<const Trk::TrackStateOnSurface>,
    std::vector<TPObjRef>,
    TrackStateOnSurfaceCnv_p3 >   TrackStateOSVectorCnv_p3;
    
  TrackStateOSVectorCnv_p3       m_trackStateVectorCnv;
};

#endif // TRACK_CNV_P3_H
