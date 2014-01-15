/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTTPCNV_COMPETINGPIXELCLUSTERSONTRACKCNV_P1_H
#define INDETEVENTTPCNV_COMPETINGPIXELCLUSTERSONTRACKCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CompetingPixelClustersOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"

#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack_p1.h"

class MsgStream;

class CompetingPixelClustersOnTrackCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
    InDet::CompetingPixelClustersOnTrack,
    InDet::CompetingPixelClustersOnTrack_p1 >
{
public:
    CompetingPixelClustersOnTrackCnv_p1() : m_cRotCnv(0) {}
    
    virtual void persToTrans( const InDet::CompetingPixelClustersOnTrack_p1   *persObj,
                                    InDet::CompetingPixelClustersOnTrack      *transObj,
                                    MsgStream                               &log );
                                    
    virtual void transToPers( const InDet::CompetingPixelClustersOnTrack      *transObj,
                                    InDet::CompetingPixelClustersOnTrack_p1   *persObj,
                                    MsgStream                               &log );

protected:
    CompetingRIOsOnTrackCnv_p1* m_cRotCnv;
};

#endif 

