/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

@section TrtDriftCircleOnTrackToolDoc Overview

When making the TRT RIO_OnTrack, the tool currently provides
<ul>
<li> a check if the prediction is in the active region, otherwise the
     DriftCircleOnTrack making is cancelled,</li>
<li> the z coordinate (obtained directly from the Track Parameters),</li>
<li> the left-right solution by setting TRT::SIDE and assigning a sign to R.
     This left-right solution takes the sign information from the
     TrackParameters which are represented as Trk::AtaStraightLine
     in the case of a DriftCircle. The sign definition is detailed in
     Trk::AtaStraightLine.</li>
</ul>

A good drift radius calibration needs to be available already at the
level of pattern recognition, therefore it is done in the
InDet::TRT_DriftFunctionTool and not in the TRT_DriftCircleOnTrackTool.

However, a more sophisticated r-T calibration is planned for this tool
to take into account signal run time and possibly wire sagging
and the r-T relation for individual tubes.

More general information on <nop>RIO_OnTrackCreation and instructions
how to use these tools can be found under Trk::RIO_OnTrackCreator.

@section TrtDriftCircleOnTrackToolDocMgr Package Managers
The TRT_DriftCircleOnTrackTool package is currently maintained by
I. Gavrilenko.
*/
