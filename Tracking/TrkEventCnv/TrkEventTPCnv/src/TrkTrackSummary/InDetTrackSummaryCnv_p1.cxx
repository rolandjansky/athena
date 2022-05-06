/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/InDetTrackSummaryCnv_p1.h"

void InDetTrackSummaryCnv_p1::persToTrans( const Trk::InDetTrackSummary_p1 *,
                                            Trk::InDetTrackSummary    *, MsgStream &/*log*/ )
{

}


void InDetTrackSummaryCnv_p1::transToPers( const Trk::InDetTrackSummary    *,
                                            Trk::InDetTrackSummary_p1 *, MsgStream &/*log*/ )
{

}
