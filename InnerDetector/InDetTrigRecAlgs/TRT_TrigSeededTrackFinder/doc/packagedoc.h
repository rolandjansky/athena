/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_TrigSeededTrackFinder_page TRT_TrigSeededTrackFinder
@author Iwona.Grabowska@cern.ch
@section TRT_TrigSeededTrackFinder_TRT_TrigSeededTrackFinderIntro Introduction

This is the algorithm for driving the back tracking through the Si part of the ID.
The input are TRT track segments or TRT xKalman stand alone tracks (configured via
the job options), and the output are global ID tracks.

This algorithm performs the following tasks:

<ul>
<li>  Loops over the input tracks and for each it calls the Si extension finder.</li>
<li>  Loops over the list of possible Si extensions to every input TRT track/segment
    and removes duplicate tracks and tracks that have too many shared hits.</li>
<li>  The ambiguity resolved Si extensions are then merged with the TRT segment to
    form a global ID track with a pseudomeasurement added at the end of the Si part
    according to the number of Si hits recovered.</li>
<li>  If no Si extension is found, the TRT track segment only is transformed into a track.
    A pseudo-measurement is added at the beginning.</li>
<li>  This new track with/without Si extension is then carefully refitted using the specified
    track fitter.</li>
</ul>



*/
