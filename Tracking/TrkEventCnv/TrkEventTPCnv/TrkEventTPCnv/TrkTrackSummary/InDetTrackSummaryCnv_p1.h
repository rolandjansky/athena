/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   InDetTrackSummaryCnv_p1.h
// author: Thijs Cornelissen
//
//-----------------------------------------------------------------------------

#ifndef TRKEVENTTPCNV_INDETTRACKSUMMARY_CNV_P1_H
#define TRKEVENTTPCNV_INDETTRACKSUMMARY_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/InDetTrackSummary_p1.h"

class MsgStream;

class InDetTrackSummaryCnv_p1: 
     public T_AthenaPoolTPCnvBase< Trk::InDetTrackSummary, Trk::InDetTrackSummary_p1 >
{
    public:
        InDetTrackSummaryCnv_p1() {}
        virtual void persToTrans( const Trk :: InDetTrackSummary_p1 *persObj,
                                        Trk :: InDetTrackSummary    *transObj,
                                        MsgStream                        &log );
        virtual void transToPers( const Trk :: InDetTrackSummary    *transObj,
                                        Trk :: InDetTrackSummary_p1 *persObj,
                                        MsgStream                        &log );
};

#endif // TRACK_SUMMARY_CNV_P1_TRK_H
