/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_TrackSegmentsTool_xk_page TRT_TrackSegmentsTool_xk
@author Igor.Gavrilenko@cern.ch
@section TRT_TrackSegmentsTool_xk_TRT_TrackSegmentsTool_xkIntro Introduction

This tool is concrete implementation for ITRT_TrackSegmentsMaker and is devoted for TRT track segments production.

TRT_TrackSegmentsMaker_CTBxk provide TRT track segments production for simple CTB and cosmic run setups where
TRT works without magnetic field and contains only straight line reconstruction algorithm. 
This algorithm uses IRIO_OnTrackCreator tool.

TRT_TrackSegmentsMaker_ECcosmics reconstructs track segments in the TRT endcaps only for cosmic runs. It uses a straight line
reconstruction algorithm in z-phi and is largely based on TRT_TrackSegmentsMaker_CTBxk.

TRT_TrackSegmentsMaker_ATLxk provide TRT track segments production for official Atlas geometry for track
originated from beam interaction region and has two step of the reconstrution global and local.

Global pattern recognition step uses the Hough transformation and historamming method and  
local step contains straight linde finding algorithm.
This algorithm uses IMagneticFieldTool, IPropagator, ITRT_DetElementsRoadMaker and ITRT_TrackExtensionTool tools.


*/
