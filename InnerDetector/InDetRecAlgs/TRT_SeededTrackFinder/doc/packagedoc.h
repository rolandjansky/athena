/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_SeededTrackFinder_page TRT_SeededTrackFinder
@author Thomas.Koffas@cern.ch

@section TRT_SeededTrackFinder_TRT_SeededTrackFinderIntro Introduction

This is the algorithm for driving the back tracking through the Si part of the ID.
The input are TRT track segments and the output are global ID tracks.

@section TRT_SeededTrackFinder_TRT_SeededTrackFinderDescription Description

This algorithm performs the following tasks:

1.  Loops over the input TRT track segments and for each it calls the Si extension finder.
    Only Segments with more than 9 drift circles will be investigated.

2.  Each of the Si extensions found is then merged with the TRT segment to
    form a global ID track with a pseudomeasurement added at the end of the Si part
    if the number of Si clusters is less than 4.

3.  If no Si extension is found, the TRT track segment only is transformed into a track.
    A pseudo-measurment is added at the beginning.This is optional and the default setting
    is false for now. It can be set via the job options.

4.  There is the option, set via job options, of refitting all the new tracks with/without Si 
    extension. The track fitter can be specified via the job options.

5.  Finally there is also the option of doing some detailed track statistics at the end of each
    event, set again via the job options. Some global statistics, such as the total number of
    tracks found, the number of TRT segments, the number of tracks with and without a Si
    component, will be output in any case.

@section TRT_SeededTrackFinder_TRT_SeededTrackFinderTools Used Tools

This algorithm uses the ITRT_SeededTrackFinderTool, ITrackFitter tools.

@section TRT_SeededTrackFinder_TRT_SeededTrackFinderContents Algorithm Contents

The TRT_SeededTrackFinder contains the following classes/files:

- TRT_SeededTrackFinder.cxx...Main class calling all the necessary tools and producing the ouput track collection.




*/
