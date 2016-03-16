/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_SUMMARY_CNV_P2_TRK_H
#define TRACK_SUMMARY_CNV_P2_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummary_p2.h"

class MsgStream;

class TrackSummaryCnv_p2: 
public T_AthenaPoolTPCnvBase< Trk::TrackSummary, Trk::TrackSummary_p2 > {
private:
    void dbgPrint(const Trk :: TrackSummary *transObj);
public:
    TrackSummaryCnv_p2() {}
    virtual void persToTrans( const Trk :: TrackSummary_p2 *persObj, Trk :: TrackSummary    *transObj, MsgStream &log );
    virtual void transToPers( const Trk :: TrackSummary    *transObj, Trk :: TrackSummary_p2 *persObj, MsgStream &log );
};

#endif