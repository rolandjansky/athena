/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrackSummaryHelperTool_page InDetTrackSummaryHelperTool Package

@author Martin Siebel <Martin.Siebel@CERN.ch>

@section InDetTrackSummaryHelperTool_InDetTrackSummaryHelperTool Introduction

The InDetTrackSummaryHelperTool is a helper tool which performs Inner Detector specific tasks for the Trk::TrackSummaryTool. Two methods can be accessed from outside: analyse() and searchForHoles().

The method analyse() basically takes a RIO_OnTrack, a Trk::TrackStateOnSurface, a vector of integers and a bitset. The RIO and the TSOS are true input quantities, the vector and the bitset are modified by analyse() and serve as output. Elements of the vector represent counters for different Trk::TrackSummary information. The analyse() methods increases these counters according to the information obtained from the other input quantities with help of the InDet ID helpers.


*/
