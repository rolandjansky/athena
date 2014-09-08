/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_MUONTRACKSUMMARY_CNV_P2_H
#define TRKEVENTTPCNV_MUONTRACKSUMMARY_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummary_p2.h"

class MsgStream;

class MuonTrackSummaryCnv_p2: 
     public T_AthenaPoolTPCnvBase< Trk::MuonTrackSummary, Trk::MuonTrackSummary_p2 >
{
    public:
        MuonTrackSummaryCnv_p2() {}
        virtual void persToTrans( const Trk :: MuonTrackSummary_p2 *persObj,
                                        Trk :: MuonTrackSummary    *transObj,
                                        MsgStream                        &log );
        virtual void transToPers( const Trk :: MuonTrackSummary    *transObj,
                                        Trk :: MuonTrackSummary_p2 *persObj,
                                        MsgStream                        &log );
};

#endif // TRACK_SUMMARY_CNV_P1_TRK_H
