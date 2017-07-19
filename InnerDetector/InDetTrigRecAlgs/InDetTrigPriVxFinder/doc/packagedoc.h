/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrigPriVxFinder_page InDetTrigPriVxFinder

@section InDetTrigPriVxFinder_introduction Introduction: InDetTrigPriVxFinder is a package
which contains algorithm(s) to calculate the primary vertex of an
event and to store the results to storegate. It makes use of the
Billoir fitting method.  All units are in mm and MeV.

@section InDetTrigPriVxFinder_howitworks How it works: The algorithm TrigVxPrimary.cxx is a
prototype usecase of the mathematical billoir fit AlgTools of the
VxBilloirTools package. It is a typical <i>tear down</i> algorithm. It
reads all tracks from StoreGate, applies some quality cut based track
selection and feeds the fit AlgTool with all tracks which passed. The
billoir tool stores the result in a VxCandidate object (see package
Tracking/TrkEvent/VxVertex) which can be accessed by TrigVxPrimary. After
a first fit, TrigVxPrimary loops again through tracks and selects them
according to their \f$ \chi^{2} \f$ of the vertex fit. Tracks with too
high \f$ \chi^{2} \f$ are rejected and the vertex is refittet. The
result is retrieved and all info is written to StoreGate.<br> <br>
There are two versions on how the algorithm handles the \f$ \chi^{2}
\f$ cut:<br> <ul> <li>Version 1: (chi2CutMethod = 1)</li> <ul> <li>get
rid of all tracks with \f$ \chi^{2} \f$ > m_maxChi2PerTrack in one
go</li> <li>no refit after removing one track</li> <li>refit after all
tracks with too high \f$ \chi^{2} \f$ were removed</li> </ul>
<li>Version 2: (chi2CutMethod = 2)</li> <ul> <li>get rid of tracks one
by one starting with the one with highest \f$ \chi^{2} \f$ >
m_maxChi2PerTrack</li> <li>refit after this track has been removed<br>
and do a \f$ \chi^{2} \f$ cut again until all \f$ \chi^{2} \f$ <
m_maxChi2PerTrack</li> </ul> </ul> The track quality cuts \f$ ( p_t,
d_0, \frac{d_0}{\sigma_{d_0}}, z_0, \chi^{2} ) \f$ can be set via
jobOptions as well as the billoir method to be used (FullFit/FastFit).

The algorithm monitors numbers of input tracks and of
reconstructed vertices. Further information about the underlying tool
can be found in InDetPriVxFinder.
 
@section InDetTrigPriVxFinder_packagecontent Package Contents:
InDetTrigPriVxFinder contains the following files/classes:
- TrigVxPrimary.cxx ... the algoritm which steers the vertex fit



\namespace Vtx
A namespace for all vertexing packages and related stuff.
*/
