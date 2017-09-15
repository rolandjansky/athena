/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetAmbiTrackSelectionTool_page InDetAmbiTrackSelectionTool Package

>> Please enter an overview of the created package here: What does it contain?

@author Martin Siebel <Martin.Siebel@CERN.ch>

@section InDetAmbiTrackSelectionTool_InDetAmbiTrackSelectionToolIntro Introduction

This tool is a helper tool to the ambiguity processing. 
It decides which hits remain on a track and which hits have to be removed, because they are already used on other tracks. 
Shared hits are allowed within certain constraints. For the book keeping which hits are alreasy used, a PRD_AssociationTool
is used. Some inline methods of the selection tool allow the client to use some functionality of the association tool via the
selection tool.

The treatment of a track works as follows:

Only the TrackStatesOnSurfaces which contain measurements with rios are considered.
All TSOS which are not from InnerDetector or which are Outliers are kept, but do not
contribute to the hit count of the track.
Other TSOS which are not yet used by other tracks are kept as 'unused' and contribute to
the 'unused' hit count. Si hits and TRT hits are counted with different weights.
Si TSOS which are already used by other tracks are kept as 'shared' and contribute to the
'shared' hit count.
TRT shared hits are not allowed.

If a track has enough hits and not more shared hits than allowed, it is returned as it is.
If a track has more shared hits than allowed, as many shared hits as allowed are added to the unused hits
and a new track is constructed from these hits.
If this track is sufficiently good, it is returned, otherweise zero is returned.



*/
