/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>
// -*- c++ -*-
// InDetSecVtxTrackAccessor.cxx
// implementations of accessor objects

#include "InDetSecVtxTrackAccessor.h"
#include "InDetSecVtxTrackSelectionTool/InDetSecVtxTrackSelectionTool.h"

// ---------------- SecVtxTrackAccessor ----------------
InDet::SecVtxTrackAccessor::SecVtxTrackAccessor(const asg::IAsgTool* tool) 
 : asg::AsgMessaging::AsgMessaging(tool)
{ }

InDet::SecVtxTrackAccessor::~SecVtxTrackAccessor() = default;

// ---------------- SummaryAccessor ----------------
InDet::SummaryAccessor::SummaryAccessor(const asg::IAsgTool* tool)
  : SecVtxTrackAccessor::SecVtxTrackAccessor(tool)
  , m_summaryValue(0)
  , m_summaryType(xAOD::numberOfTrackSummaryTypes)
{
}

void InDet::SummaryAccessor::setSummaryType( xAOD::SummaryType sumType )
{
  m_summaryType = sumType;
  return;
}

StatusCode InDet::SummaryAccessor::access( const xAOD::TrackParticle& track,
                                           const xAOD::Vertex* )
{
  if (!track.summaryValue(m_summaryValue, m_summaryType)) {
    ATH_MSG_DEBUG( "Failed to get SummaryType " << std::to_string(m_summaryType) 
       << " from xAOD::TrackParticle summary. A value of zero will be used instead." );
    m_summaryValue = 0;
  }
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::SummaryAccessor::access ( const Trk::Track&,
                                            const Trk::TrackParameters*,
                                            const Trk::TrackSummary* summary )
{
  if (!summary) {
    ATH_MSG_ERROR( "Recieved null pointer to track summary." );
    m_summaryValue = 0;
    return StatusCode::FAILURE;
  }
  // previously casting the xAOD::SummaryType directly to a Trk::SummaryType.
  // now casting to an int in between to try to remove coverity warning.
  Int_t summaryTypeInt = static_cast<Int_t>(m_summaryType);
  Int_t checkSummaryValue = summary->get( static_cast<Trk::SummaryType>(summaryTypeInt) );
  if (checkSummaryValue < 0) {
    // Trk::TrackSummary::get() will return -1 if the data cannot be retrieved
    ATH_MSG_DEBUG( "Recieved " << checkSummaryValue << " for SummaryType " << std::to_string(m_summaryType)
           << " from Trk::TrackSummary. A value of zero will be used instead." ); 
    checkSummaryValue = 0;
  }
  m_summaryValue = checkSummaryValue;
  return StatusCode::SUCCESS;
}
#endif

