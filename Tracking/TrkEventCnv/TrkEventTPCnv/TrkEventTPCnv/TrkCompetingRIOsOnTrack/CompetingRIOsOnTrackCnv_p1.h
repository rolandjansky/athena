/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_COMPETINGRIOONTRACKCNV_P1_H
#define TRKEVENTTPCNV_COMPETINGRIOONTRACKCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CompetingRIOsOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

class MsgStream;

class CompetingRIOsOnTrackCnv_p1
   : public T_AthenaPoolTPAbstractPolyCnvBase< Trk::MeasurementBase, Trk::CompetingRIOsOnTrack, Trk::CompetingRIOsOnTrack_p1 >
{
public:
  CompetingRIOsOnTrackCnv_p1() : m_localParCnv(0), m_errorMxCnv(0) {}
  void persToTrans( const Trk::CompetingRIOsOnTrack_p1 *persObj, Trk::CompetingRIOsOnTrack *transObj, MsgStream &log );
  void transToPers( const Trk::CompetingRIOsOnTrack *transObj, Trk::CompetingRIOsOnTrack_p1 *persObj, MsgStream &log );
  
private:
    LocalParametersCnv_p1   *m_localParCnv;
    ErrorMatrixCnv_p1       *m_errorMxCnv;
};

#endif // RIO_ON_TRACK_CNV_P1_H
