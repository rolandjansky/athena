/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_StandaloneTrackFinder_page TRT_StandaloneTrackFinder
@author Thomas.Koffas@cern.ch

@section TRT_StandaloneTrackFinder_TRT_StandaloneTrackFinderIntro Introduction

This is an algorithm that deals with the TRT segments that have not
been assigned a Si extension. It runs after the TRT seeded back-tracking
is completed, including the ambiguity resolving of the resulting tracks.
The input are TRT track segments and the output are global ID tracks.

@section TRT_StandaloneTrackFinder_TRT_StandaloneTrackFinderDescription Description

This algorithm performs the following tasks:

1.  Loops over the input TRT track segments and checks the number of 
    the segment's drift circles that have been already used by the Si
    extended back-tracks, using the association tool after the later
    has been updated during the ambiguity resolving of the back-tracks.
    Only segments with more than 15 drift circles (configurable via the
    algorithm's jobOptions) will be investigated. Those TRT track segments
    that are found to share more than half (also configurable via the
    algorithm's jobOptions) of their drift circles with previously
    reconstructed back-tracks are dropped.

2.  Each of the TRT track segments that pass the above criteria are then fitted
    using the configured track fitter. Those that are successfully fitted are 
    then stored in a dedicated track collection.

@section TRT_StandaloneTrackFinder_TRT_StandaloneTrackFinderTools Used Tools

This algorithm uses the ITrackFitter, InDetAssociationTool tools.

@section TRT_StandaloneTrackFinder_TRT_StandaloneTrackFinderContents Algorithm Contents

The TRT_StandaloneTrackFinder contains the following classes/files:

- TRT_StandaloneTrackFinder.cxx...Main class calling all the necessary tools and producing the ouput track collection.




*/
