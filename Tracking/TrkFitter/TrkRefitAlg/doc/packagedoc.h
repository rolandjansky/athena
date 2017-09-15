/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkRefitAlg_page TrkRefitAlg Package
@author T. Cornelissen, M.J. Costa, W. Liebig

@section TrkRefitAlg_TrkRefitAlgIntro Introduction

This package provides a configurable Algorithm, Trk::ReFitTrack,
to refit tracks.
In can be used to refit tracks with a different fitter and/or
track model, to re-create track parameters at each measurement
previously lost through data conversion or compression, and to
merge tracks from two single-track collections.
The output is written to a new track collection.

Note that this package is not meant to provide a full reconstruction
strategy. For example combined track fits or improved fit strategies
after particle identification should be implemented inside the
offline Reconstruction CVS realm.

@section TrkRefitAlg_TrkRefitAlgSteering Steering the Trk::ReFitTrack
Trk::ReFitTrack is set up using standard Gaudi/Athena configurables
@htmlonly
<table cellspacing=10px><tr>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
<td style="background-color:#e8e0e8; color:#000000;
           padding:10px;" >
<pre>
from TrkRefitAlg.TrkRefitAlgConf import Trk__ReFitTrack
MyReFitTrack = Trk__ReFitTrack (name           = "MyRefitTrack",
                                FitterTool     = ... )
topSequence += MyReFitTrack
</pre></td>
</tr></table>
@endhtmlonly
 
@section TrkRefitAlg_ExtrasTrkRefitAlg Extra Pages

*/
