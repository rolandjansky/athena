/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkTrackSlimmingTool_page TrkTrackSlimmingTool Package

This package provides the tool to 'slim' a single Track, i.e. remove
(possibly redundant) information from it in order to reduce its
size before writing to disk.

By default, the slimmed track will include all the measurements,
and the perigee parameters (currently these are assumed to exist,
but eventually an extrapolator should be used to provide it if the
perigee is missing). Also the track's author informationa and fit
quality are retained. Steered by job options, additional information
can be included in the slimmed track, for instance the track parameters
at the first and last detector surface in order to maintain the
precision for using this track in combination and matching with
other detectors, and in vertexing.

The slimming is done technically by selecting interesting information
from the passed track, and by creating a new track with cloned copies
of this information. This concept allows to keep the principal
track collection in memory with full detail information (eg for
hit calibration studies using the parameters at every detector surface)
while only the new, slimmed track collection is selected for disk
storage.

@author Edward Moyse <Edwar.Moysecern.ch>

@section TrkTrackSlimmingTool_joboTrkTrackSlimmingTool Job Options for TrackSlimmingTool
<DL>
  <DT>KeepOutliers
  <DD>If this is set to true, Outliers will be kept on the slimmed track.
      <i>Default: false</i>
  <DT>KeepParameters
  <DD>If this is set to true, the first and last parameters will be kept on the slimmed track. <i>Default: false</i>
</DL>

@section TrkTrackSlimmingTool_ExtrasTrkSlimmingTool Extra Pages

*/
