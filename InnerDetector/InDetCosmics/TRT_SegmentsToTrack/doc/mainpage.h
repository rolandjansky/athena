/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage Segment to track converter for Cosmics

@section IntroductionTRT_SegmentsToTrack Introduction

This package converts TRT segments into full tracks by feeding them into a TrackFitter. At the same time,
it can try to combine the TRT segment with a Si only track to produce combined tracks.

Another functionality has been introduced for the TRT calibration: the algorithm can (if selected via jobOptions) try to perform
the eventphase correction and the per wheel t0 correction (TRT endcap).


@author Christian.Schmitt@cern.ch

@section requirementsTRT_SegmentsToTrack Requirements
@include requirements

*/
