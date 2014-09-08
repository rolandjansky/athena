/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_CNV_P12_H
#define TRACK_CNV_P12_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkTrack/Track.h"
#include "TrkEventTPCnv/TrkTrack/Track_p12.h"

// #include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p1.h"

class MsgStream; 

class TrackCnv_p12: public T_AthenaPoolTPCnvBase<Trk::Track, Trk::Track_p12>
{
public:
  TrackCnv_p12(){} // : m_fqCnv(0) {}

  void persToTrans( const Trk::Track_p12* persObj, Trk::Track* transObj, MsgStream& );
  void transToPers( const Trk::Track* transObj, Trk::Track_p12* persObj, MsgStream& );

  //virtual 
  void	initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
     m_trackStateVectorCnv.setTopConverter( topCnv, 0 );
  }
  
protected:
  typedef T_AthenaPoolTPPtrVectorCnv< DataVector<const Trk::TrackStateOnSurface>,
				      std::vector<TPObjRef>,
				      TrackStateOnSurfaceCnv_p1 >   TrackStateOSVectorCnv_p1;
  
  // FitQualityCnv_p1           *m_fqCnv;
  TrackStateOSVectorCnv_p1	m_trackStateVectorCnv;
};

#endif 
