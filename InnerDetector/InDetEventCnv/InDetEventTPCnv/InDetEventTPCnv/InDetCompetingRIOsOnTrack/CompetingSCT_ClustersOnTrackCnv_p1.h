/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTTPCNV_COMPETINGSCT_CLUSTERSONTRACKCNV_P1_H
#define INDETEVENTTPCNV_COMPETINGSCT_CLUSTERSONTRACKCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CompetingSCT_ClustersOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"

#include "InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack_p1.h"

class MsgStream;

class CompetingSCT_ClustersOnTrackCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
    InDet::CompetingSCT_ClustersOnTrack,
    InDet::CompetingSCT_ClustersOnTrack_p1 >
{
public:
    CompetingSCT_ClustersOnTrackCnv_p1() : m_cRotCnv(0) {}
    
    virtual void persToTrans( const InDet::CompetingSCT_ClustersOnTrack_p1   *persObj,
                                    InDet::CompetingSCT_ClustersOnTrack      *transObj,
                                    MsgStream                               &log );
                                    
    virtual void transToPers( const InDet::CompetingSCT_ClustersOnTrack      *transObj,
                                    InDet::CompetingSCT_ClustersOnTrack_p1   *persObj,
                                    MsgStream                               &log );

protected:
    CompetingRIOsOnTrackCnv_p1* m_cRotCnv;
};

#endif 

