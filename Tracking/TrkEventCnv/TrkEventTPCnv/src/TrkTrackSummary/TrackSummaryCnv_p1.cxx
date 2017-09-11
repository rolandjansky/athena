/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummaryCnv_p1.h"

void TrackSummaryCnv_p1::persToTrans( const Trk::TrackSummary_p1 *persObj,
                                            Trk::TrackSummary    *transObj, MsgStream &log )
{
   transObj->m_information       = persObj->m_information;
   if (transObj->m_information.size() < Trk::numberOfTrackSummaryTypes)
     transObj->m_information.resize(Trk::numberOfTrackSummaryTypes,
                                    Trk::TrackSummary::SummaryTypeNotSet);
    if (persObj->m_information.size() <= Trk::numberOfInnermostPixelLayerHits) {
      transObj->m_information[Trk::numberOfInnermostPixelLayerHits] =
        transObj->m_information[Trk::numberOfBLayerHits];
    }

    if (persObj->m_information.size() <= Trk::numberOfInnermostPixelLayerSharedHits) {
      transObj->m_information[Trk::numberOfInnermostPixelLayerSharedHits] =
        transObj->m_information[Trk::numberOfBLayerSharedHits];
    }

   transObj->m_idHitPattern      = persObj->m_idHitPattern;
   transObj->m_eProbability      = persObj->m_eProbability;
   transObj->m_indetTrackSummary = createTransFromPStore( &m_indetSummaryCnv, persObj->m_indetTrackSummary, log );
   transObj->m_muonTrackSummary  = createTransFromPStore( &m_muonSummaryCnv, persObj->m_muonTrackSummary, log );
   transObj->m_dedx              = persObj->m_pixeldEdx;
   transObj->m_nhitsdedx         = persObj->m_nhitsfordEdx;
}


void TrackSummaryCnv_p1::transToPers( const Trk::TrackSummary    *transObj,
                                            Trk::TrackSummary_p1 *persObj, MsgStream & log )
{
   persObj->m_information       = transObj->m_information;
   persObj->m_idHitPattern      = transObj->m_idHitPattern;
   persObj->m_eProbability      = transObj->m_eProbability;
   persObj->m_indetTrackSummary = toPersistent( &m_indetSummaryCnv, transObj->m_indetTrackSummary, log );
   persObj->m_muonTrackSummary  = toPersistent( &m_muonSummaryCnv, transObj->m_muonTrackSummary, log );
   persObj->m_pixeldEdx         = transObj->m_dedx;
   persObj->m_nhitsfordEdx      = transObj->m_nhitsdedx;
}
