/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_SeededTrackFinderTool_page TRT_SeededTrackFinderTool
@author Thomas.Koffas@cern.ch

@section TRT_SeededTrackFinderTool_TRT_SeededTrackFinderToolIntro Introduction

This tool is concrete implementation for ITRT_SeeedTrackFinder.h
The input is a track segment, for example a TRT track segment
It returns a list of tracks that in principle span the full lenght of the Si ID.
This tool is the main logic for finding Si extensions to stand alone TRT segments.
It is called by the back TRacking algorithm TRT_SeededTrackFinder

@section TRT_SeededTrackFinderTool_TRT_SeededTrackFinderToolDescription Tool Description

The TRT seeded track finder tool contains the following steps:

1. Initialization step. This initializes both the seed finding and the combinatorial track
   finding tools that are called later by this tool during track finding. A cluster-track
   multimap, built for each event is also re-initialized.

2. For each input TRT track/segment the initial track parameters are found.They are 
   the track parameters on the first TRT drift circle of the TRT segment under consideration.
   These TRT segment track parameters are then passed to the main tracking routine in this tool.
   The rest of the description follows the steps in this main routine.

3. The Si seed finder tool is called and a list of possible SCT space point pair seeds is found,
   using the above mentioned track parameters.

4. For further use the TRT track parameters are modified so that the covariance matrix is diagonal.
   A possible error scaling is also allowed although not used for the moment.The TRT segment theta
   prediction is modified, using the one suggested by the slope of the two space points in the 
   seed currently under consideration.

5. Start looping over the space point seeds.

6. The seed will be dropped from further consideration if any of the following is true:

   i) Both space points have already been used by another laready found track in this event.

   ii)Both space points have already been used by other high quality (>9 hits) tracks.

   iii)The seed contains only one space point due to instabilities caused during propagation. 

7. An explicit propagation through the two space points of the seed then takes place using the 
   modified TRT track parameters with the diagonal covariance matrix.The seed again will be dropped if:

   i) The extrapolation from one space point to the other failed for any reasons.

   ii)There are outliers based on a chi2 cut of 50.

   Otherwise new track parameters are produced at the end more accurate than those obtained from the
   TRT track segment.

8. Using the new more accurate track parameters at the perigee, a road through the silicon elements 
   is constructed. If failure, then move to the next space point seed.

9. The updated measured track parameters after seed propagation are again modified so that the covariance
   matrix is diagonal. Error scaling is permitted although not applied for now.

10.The combinatorial track finding tool is then called using the track parameters from the previous
   step along with the Si element road and the list of space points (ordered inside-out). A set of
   track quality cuts configured via job options is also passed to this tool. A list of Si only
   track extensions is returned.

11.These tracks are inspected and possibly cleaned from spurious pixel hits collected. This is
   particularly important to determine the correct starting cluster of each track.

12.Duplicate tracks are also removed. The cluster-track map is then updated using all the remaining tracks.
   These are the returned tracks by this routine.

13.If no tracks are found during the previous steps, then there is the option for brem correcting the
   initial TRT segment track parameters and repeat the track search. This is decided again via job options.
   It is set to false by default.

@section TRT_SeededTrackFinderTool_TRT_SeededTrackFinderToolTools Used Tools

This tool uses IMagneticFieldTool, IPropagator, IUpdator, ISiDetElementsRoadMaker, ITRT_SeededSpacePointFinder,
ISiCombinatorialTrackFinder tools.

@section TRT_SeededTrackFinderTool_TRT_SeededTrackFinderToolContents Tool Contents

The TRT_SeededTrackFinderTool contains the following classes/files:

- TRT_SeededTrackFinder_ATL.cxx... Main track finding algorithm

- SiNoise_bt.cxx... Noise adding routine due to detector material




*/
