/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage The TrkTrackSummaryTool  

@section introductionTrkTrackSummaryTool Overview
This AlgTool uses the InDet::InDetTrackSummaryHelperTool and/or MuonTrackSummaryHelperTool to create a Trk::TrackSummary object from the passed Trk::RIO_OnTrack. For it to work, at least one of these other tools must have been loaded into memory (i.e. the library must have been loaded by jobOptions). If TrkTrackSummaryTool cannot find either of its helper tools, then it will not initialise correctly, returning StatusCode::FAILURE.

*/
