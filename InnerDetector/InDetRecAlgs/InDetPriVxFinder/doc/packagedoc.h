/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetPriVxFinder_page InDetPriVxFinder

@section InDetPriVxFinder_introduction Introduction:
InDetPriVxFinder is a package which contains algorithm(s) to calculate the primary vertex of
an event and to store the results to storegate. It makes use of the InDetPriVxFinderTools.
All units are in mm and MeV.

@section InDetPriVxFinder_howitworks How it works:
The alogorithm VxPrimary.cxx reads all tracks from StoreGate, calls one of the InDetPriVxFinderTool methods, retrieves a VxContainer
and writes all info to StoreGate.

@section InDetPriVxFinder_jobOptions The jobOptions file:
The algorithm is automaticaly configured via genConf
<ul>
<li>Specifiy the name of the track input container:<br>
<b>VxPrimary.TracksName = "Tracks";</b><br>
<i>Remark:  Only Trk::Track(s) are supported - so as long as the TrkLegacy package does not change its output
name this can be left untouched.</i></li>
<li>Specify the name of the output container:<br>
<b>VxPrimary.VxCandidatesOutputName      = "VxPrimaryCandidate";</b></li>
</ul>


\namespace Vtx
A namespace for all vertexing packages and related stuff.
*/
