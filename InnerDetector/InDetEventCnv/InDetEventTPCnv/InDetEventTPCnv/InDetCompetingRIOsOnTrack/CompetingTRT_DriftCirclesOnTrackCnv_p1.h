/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTTPCNV_COMPETINGTRT_DRIFTCIRCLESONTRACKCNV_P1_H
#define INDETEVENTTPCNV_COMPETINGTRT_DRIFTCIRCLESONTRACKCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CompetingTRT_DriftCirclesOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"

#include "InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack_p1.h"

class MsgStream;

class CompetingTRT_DriftCirclesOnTrackCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
    InDet::CompetingTRT_DriftCirclesOnTrack,
    InDet::CompetingTRT_DriftCirclesOnTrack_p1 >
{
public:
    CompetingTRT_DriftCirclesOnTrackCnv_p1() : m_cRotCnv(0) {}
    
    virtual void persToTrans( const InDet::CompetingTRT_DriftCirclesOnTrack_p1   *persObj,
                                    InDet::CompetingTRT_DriftCirclesOnTrack      *transObj,
                                    MsgStream                               &log );
                                    
    virtual void transToPers( const InDet::CompetingTRT_DriftCirclesOnTrack      *transObj,
                                    InDet::CompetingTRT_DriftCirclesOnTrack_p1   *persObj,
                                    MsgStream                               &log );

protected:
    CompetingRIOsOnTrackCnv_p1* m_cRotCnv;
};

#endif 

