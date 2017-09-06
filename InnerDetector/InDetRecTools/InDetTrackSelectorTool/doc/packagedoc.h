/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrackSelectorTool_page The InDetTrackSelectorTool package

@author Andreas.Wildauer@cern.ch
@author Daniel.Kollar@cern.ch

@section InDetTrackSelectorTool_introductionInDetTrackSelectorTool Introduction
This package contains the concrete implementation of the inner detector track selector tools. The interface can be found
in Tracking/TrkTools/TrkToolInterfaces.

@section InDetTrackSelectorTool_selectorsInDetTrackSelectorTool Selectors
The implementations in this package are

- InDetTrackSelector: has mostly cuts on perigee parameters
- InDetDetailedTrackSelector: in addition can cut on hits and other variables
- InDetConversionTrackSelector: a track selector dedicated for conversion finding 

The cuts implemented in each version are shown below.

@image html TrackSelectorVariables.png

*/
