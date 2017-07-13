/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TRT_TrackExtensionTool_DAF_page The TRT_TrackExtensionTool_DAF package
Tool to extend Silicon tracks into the TRT based on the Deterministic Annealing Filter.

@author Sebastian.Fleischmann@cern.ch

@section TRT_TrackExtensionTool_DAF_introductionTRT_TrackExtensionTool_DAF Introduction

This package contains a tool to produce a vector of InDet::CompetingTRT_DriftCirclesOnTrack
which can be used by the InDetExtensionProcessor to extend silicon tracks into the TRT.
InDet::TRT_DriftCircles lying on a road around the track prediction are choosen and grouped
into InDet::CompetingTRT_DriftCirclesOnTrack containing measurements which are not compatible
with each other. The Trk::DeterministicAnnealingFilter (DAF) will then decide in an iterative annealing procedure which InDet::TRT_DriftCircleOnTrack s are assigned to the extended track.

As the DAF TRT_TrackExtensionTool just produces a vector of Trk::MeasurementBase (resp.
InDet::CompetingTRT_DriftCirclesOnTrack) without actually deciding which measurements belong
to the track (i.e. just doing a course assignment) this tool can only be used in combination
with the Trk::DeterministicAnnealingFilter.

This tool has two operation modes. The first (simple) one just groups all measurements (TRT_DriftCircles)
on one TRT detector element into the same CompetingTRT_DriftCirclesOnTrack. The more sophisticated mode
groups measurements on "pseudo-detector surfaces" which are perpendicular to the track prediction for the
Barrel pert of the track. In the TRT endcaps the measurements are always grouped by the detector elements
because polar coordinates in the wheel plane are used. Both modes are almost identical if the tracks are
nearly perpendicular to the detector element planes as it is the case for high momentum tracks.

The following sketch shows a track prediction and how TRT measurements are grouped in the more
sophisticated mode.
@image html TRT-extension-grouped.png

Figure 2 shows the same track prediction and measurements in the case of the simple mode. The first
measurements are grouped in a sensible way, whereas the grouping fails for the last measurements, because
the track is not perpendicular to the detector elements anymore.
@image html TRT-extension-detElement-wise.png


@section TRT_TrackExtensionTool_DAF_TRT_TrackExtensionTool_DAFOverview Class Overview
  The TRT_TrackExtensionTool_DAF package contains the following class:

  - InDet::TRT_TrackExtensionTool_DAF : Tool to collect the compatible measurements
       in InDet::CompetingTRT_DriftCirclesOnTrack.

@section TRT_TrackExtensionTool_DAF_howToUseTRT_TrackExtensionTool_DAF How to use the TRT_TrackExtensionTool_DAF
The Trk::TRT_TrackExtensionTool_DAF can be configured by the Python setup class <tt>ConfiguredTRT_TrackExtensionTool_DAF</tt> which is defined in <tt><a href="ConfiguredTRT__TrackExtensionTool__DAF_8py-source.html">ConfiguredTRT_TrackExtensionTool_DAF.py</a></tt>.It is important to configure the InDetExtensionProcessor to use the TrkDeterministicAnnealingFilter as TrackFitter. This can be done by the Python setup class <tt>ConfiguredInDetExtensionProcessor</tt>.


@subsection TRT_TrackExtensionTool_DAFJobOptions jobOptions of the InDet::TRT_TrackExtensionTool_DAF
<DL>
  <DT>MagneticTool
  <DD>The magnetic field tool to get the field properties used by the propagator. Default: <tt>Trk::MagneticFieldTool_xk</tt>
  
  <DT>MagneticFieldMode
  <DD>fieldmode to be used for the field properties. Default: <tt>MapSolenoid</tt>
  
  <DT>PropagatorTool
  <DD>tool to propagate track parameters. Default: <tt>Trk::RungeKuttaPropagator</tt>
  
  <DT>CompetingDriftCircleTool
  <DD>tool to create the InDet::CompetingTRT_DriftCirclesOnTrack. Default: <tt>InDet::CompetingTRT_DriftCirclesOnTrackTool/CompetingTRT_DriftCirclesOnTrackTool</tt>
  
  <DT>RoadTool
  <DD>tool to find a road of detector elements in the TRT. Default: <tt>InDet::TRT_DetElementsRoadMaker_xk</tt>
  
  <DT>RoadWidth
  <DD>width of the road of InDet::TRT_DriftCircle s (independent of the RoadTool). Default: <tt>10.</tt>
  
  <DT>TRT_DriftCircleContainer
  <DD>StoreGate container name of the InDet::TRT_DriftCircle. Default: <tt>TRT_DriftCircles</tt>
  
  <DT>InitialAnnealingFactor
  <DD>annealing factor used to create the InDet::CompetingTRT_DriftCirclesOnTrack. Default: <tt>81.</tt>
  
  <DT>SimpleElementWiseExtension
  <DD>do a simple detector element-wise grouping of the TRT_DriftCircles or use a more
    sophisticated way in the Barrel part? See above for a detailed description. Default: <tt>false</tt>
  
  <DT>MaxGroupDistance
  <DD>maximum distance of TRT_DriftCircle groups when using the sophisticated grouping method. Default: <tt>5.</tt>

@section TRT_TrackExtensionTool_DAF_ExtrasTRT_TrackExtensionTool_DAF Extra Pages

*/
