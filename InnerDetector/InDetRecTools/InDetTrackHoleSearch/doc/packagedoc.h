/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrackHoleSearch_page InDetTrackHoleSearch Package

>> Please enter an overview of the created package here: What does it contain?

@author Martin Siebel <Martin.Siebel@CERN.ch>

@section InDetTrackHoleSearch_InDetTrackHoleSearchIntro Introduction

This package contains the InDetTrackHoleSearchTool which is derived from the Trk::ITrackHoleSearch interface. The basic functionality has been part of the InDetTrackSummaryHelperTool, but with increasing demands on functionality and complexity, has been sourced out into an AlgTool in its own right, the InDetTrackHoleSearchTool.

The method searchForHoles() basically takes a Trk:Track and a vector of integers. Elements of the vector represent counters for different Trk::TrackSummary information. The method followsthe Trk::Track through the silicon part of the Inner Detector and determines the number of sensitive detector elements crossed by the track without inducing a signal in these elements. Every such occurence is called a hole. Note, that by definition holes are only counted between measurements, i.e. holes at smaller radii than the innermost measurement are not counted as such. However, holes between the outermost SCT hit are considered, if the track has hits in the TRT.

The hole search follows the track by extrapolation from the surface of a given hit to the surface of the next given hit. The startpoint for every extrapolation step is the Trk::TrackParameter on this surface if present, otherwise the less precise extrapolation end-point is used. Sensitive surfaces crossed by the extrapolation are counted as holes. Special consideration is made for SCT hits. There is no extrapolation step from a SCT-module to its backside. Instead, it is directly checked, if the next hit of the track is located
 on the backside of the SCT-module.

In order to work smoothly, the Trk::TrackStateOnSurface objects of a track which correspond to a silicon detector are sorted along the momentum of the Trk::Track. Then all these hits and all TRT hits which are either followed by a silicon hit or follow on a silicon hit are considered for the hole search.


>> Please enter a brief description of the package here.



*/
