/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/InDetTrackSummaryCnv_p1.h"

void InDetTrackSummaryCnv_p1::persToTrans( const Trk::InDetTrackSummary_p1 *persObj,
                                            Trk::InDetTrackSummary    *transObj, MsgStream &/*log*/ )
{
    transObj->m_massdedx       = persObj->m_massdedx;
    transObj->m_likelihoodspixeldedx      = persObj->m_likelihoodspixeldedx;

}


void InDetTrackSummaryCnv_p1::transToPers( const Trk::InDetTrackSummary    *transObj,
                                            Trk::InDetTrackSummary_p1 *persObj, MsgStream &/*log*/ )
{
    persObj->m_massdedx       = transObj->m_massdedx;
    persObj->m_likelihoodspixeldedx  = transObj->m_likelihoodspixeldedx;

}
