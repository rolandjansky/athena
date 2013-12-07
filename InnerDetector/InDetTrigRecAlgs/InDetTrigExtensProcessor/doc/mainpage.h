/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage The InDetTrigExtensProcessor

@section Introduction

InDetTrigExtensProcessor is an EF trigger version of
InDetExtensionProcessor. The task of the InDetTrigExtensProcessor is
to extend existing tracks in silicon with TRT extensions.
InDetTrigExtensProcessor takes as an input a collection of tracks and
a track extension map. If a given track has no entry in the extension
map it is copied to output without modifications. Otherwise
InDetTrigExtensProcessor refits the track with extra
InDet::InDetPrepRawData from the extensions, compares the score of the
new track with the input track and stores the better one in the output
collection. If the score is worse the TRT hits are added as outliers
(controlled by keepFailedExtension). InDetTrigExtensProcessor by
default uses Trk::KalmanFitter InDetTrigTrackFitter and
Trk::TrackScoringTool InDetTrigScoringTool as underlying tools.

@htmlinclude used_packages.html

@include requirements

*/
