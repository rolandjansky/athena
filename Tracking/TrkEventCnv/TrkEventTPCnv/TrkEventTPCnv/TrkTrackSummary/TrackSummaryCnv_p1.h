/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackSummaryCnv_p1.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef TRACK_SUMMARY_CNV_P1_TRK_H
#define TRACK_SUMMARY_CNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummary_p1.h"
#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummaryCnv_p1.h"
#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummaryCnv_p2.h"

class MsgStream;

class TrackSummaryCnv_p1: 
     public T_AthenaPoolTPCnvBase< Trk::TrackSummary, Trk::TrackSummary_p1 >
{
    public:
        TrackSummaryCnv_p1() : m_muonSummaryCnv(0) {}
        virtual void persToTrans( const Trk :: TrackSummary_p1 *persObj,
                                        Trk :: TrackSummary    *transObj,
                                        MsgStream                        &log );
        virtual void transToPers( const Trk :: TrackSummary    *transObj,
                                        Trk :: TrackSummary_p1 *persObj,
                                        MsgStream                        &log );
    private:
        MuonTrackSummaryCnv_p1               *m_muonSummaryCnv;  
        // MuonTrackSummaryCnv_p2               *m_muonSummaryCnv_p2;  
};

#endif // TRACK_SUMMARY_CNV_P1_TRK_H
