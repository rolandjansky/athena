/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFT_CIRCLE_ON_TRACK_CNV_P1_H
#define TRT_DRIFT_CIRCLE_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrack_p1.h"

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"

#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModel/ElementLink.h" 
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"


class MsgStream;

class TRT_DriftCircleOnTrackCnv_p1
    : 
    public T_AthenaPoolTPPolyCnvBase<   Trk::MeasurementBase,
                                        InDet::TRT_DriftCircleOnTrack,
                                        InDet::TRT_DriftCircleOnTrack_p1 >
{
public:
    TRT_DriftCircleOnTrackCnv_p1() : m_RIOCnv(0) {}

    void persToTrans(   const InDet :: TRT_DriftCircleOnTrack_p1 *persObj,
                        InDet :: TRT_DriftCircleOnTrack    *transObj,
                        MsgStream                          &log );
    void transToPers(   const InDet :: TRT_DriftCircleOnTrack    *transObj,
                        InDet :: TRT_DriftCircleOnTrack_p1 *persObj,
                        MsgStream                          &log );

protected:
    RIO_OnTrackCnv_p1	*m_RIOCnv;
    ElementLinkCnv_p1< ElementLinkToIDCTRT_DriftCircleContainer >     m_elCnv;     

};

#endif // DRIFT_CIRCLE_ON_TRACK_CNV_P1_H
