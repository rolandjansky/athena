/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonClusterOnTrackCreator_page 

@section MuonClusterOnTrackCreator_MuonClusterOnTrackToolDocIntro Overview

This package provides a tool, MuonClusterOnTrackTool, which turns
Muon::MuonCluster into Muon::MuonClusterOnTrack objects
using a given trajectory prediction. The Muon::MuonCluster class
groups measurements originating from the following detector
technologies:
<ul>
<li> CSC - Cathode Strip Chambers, in the far endcap</li>
<li> RPC - Resistive Plate Chambers, in the barrel</li>
<li> TGC - Thin Gap Chambers, in the endcap</li>
</ul>

In the current version of this tool, no corrections are actually
performed for RPC or TGC, but the information is
only copied into the corresponding Trk::RIO_OnTrack objects.
An individual error scaling can be applied at this point separately
for each of the three technologies. The error scaling is controlled
centrally by jobOptions in the Trk::RIO_OnTrackCreator package.
It is, of course, switched off by default.

More general information on RIO_OnTrackCreation and instructions
how to use these tools can be found under Trk::RIO_OnTrackCreator.

@section MuonClusterOnTrackCreator_MuonClusterOnTrackToolDocCsc CSC clusters

The position resolution for CSC clusters depend on the angle of
incidence and this tool provides means to correct the measurement
errors accordingly, making use of a CSC cluster fitting tool.

@section MuonClusterOnTrackCreator_MuonClusterOnTrackToolConfiguration Configuration

The tool is configured in python. Use the following to access the default
instance:
<pre>
from MuonClusterOnTrackCreator.DefaultMuonClusterOnTrackCreator import theMuonClusterOnTrackCreator
</pre>


@section MuonClusterOnTrackCreator_MuonClusterOnTrackToolDocMgr Package Managers
The MuonClusterOnTrackTool package is currently maintained by
N. van Eldik and W. Liebig.

*/
