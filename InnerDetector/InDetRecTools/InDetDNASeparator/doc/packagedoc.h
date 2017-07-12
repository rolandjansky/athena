/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetDNASeparator_page InDetDNASeparator Package

@author Vato Kartvelishvili <vakhtang.kartvelishvili@cern.ch>
@author Wolfgang Liebig  <http://consult.cern.ch/xwho/people/54608>

@section InDetDNASeparator_InDetDNASeparatorIntro Summary
Evaluate track for energy-loss type breakpoints based on
existing material effects and momentum information on the track.
It analyses the momentum evolution along the trajectory, comparing
the momenta at different 'floors', which are detector layers
sufficently spatially separated to allow sensitivity to momentum
change. It can be used as module inside a fitter as well as re-analysis
of readily fitted Trk::Track objects.

One type of output from the brem analysis is a significance value
for potential brem, a variable which ranges from -100 (no brem or a problem)
via small negative values (energy increase due to resolution effects)
to high positive values (significant brem). The other type
of output is a decision: no breakpoint, insignificant breakpoint
or a confirmed, significant breakpoint. Breakpoint means brem
for this tool.

@section InDetDNASeparator_InDetDNASeparatorTechnical Implementation
The DNA separator implements the Trk::ITrackBreakpointAnalyser
interface from the common tracking realm. It has two sets of
methods: one performs the analysis on input from the internal
data members of track fitters (std::vector<Trk::ProtoTrackStateOnSurface>)
the other one on full EDM Trk::Tracks.
Each set exists in one method to calculate the significance of the brem
and another one performing a decision based on this significance.

This package also has an internal data class: <nop>MeanTrackParsAtFloor which
manages the representation of any combination of measurements on
brem-sensitive detector floors.

@section InDetDNASeparator_InDetDNASeparatorJO jobOptions

<table>
<tr><td><b>option</b></td><td><b>default</b></td><td><b>purpose</b></td></tr>
<tr><td>kutoff</td><td>4.50</td><td>significance value above which a brem is
  declared as 'confirmed'.</td></tr>
</table>

@section InDetDNASeparator_InDetDNASeparatorExtras Extra Pages

*/
