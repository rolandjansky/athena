/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage The TrkCompetingRIOsOnTrackTool package
The package for CompetingRIOsOnTrack creation.

@author Sebastian.Fleischmann@cern.ch

@section introductionTrkCompetingRIOsOnTrackTool Introduction

This package contains the tools to create Trk::CompetingRIOsOnTrack. This includes
    creation of the RIO_OnTrack objects with the corresponding tools.
    The actual calculations are not done in this package, but are
    delegated to the detector-specific packages.

@section TrkCompetingRIOsOnTrackToolOverview Class Overview
  The TrkCompetingRIOsOnTrackTool package contains the following classes:

  - Trk::ICompetingRIOsOnTrackTool : Interface class for CompetingRIOsOnTrackTools.
  - Trk::CompetingRIOsOnTrackTool : creates Trk::CompetingRIOsOnTrack objects using a given
    track hypothesis and a list of Trk::PrepRawData using the detector specific sub-tool.
  - Trk::IWeightCalculator : Interface class for WeightCalculators, which compute the assignment probabilities of competing measurements to a track
  
@section ExtrasTrkCompetingRIOsOnTrackTool Extra Pages

 - @ref UsedTrkCompetingRIOsOnTrackTool
 - @ref requirementsTrkCompetingRIOsOnTrackTool
*/

/**
@page UsedTrkCompetingRIOsOnTrackTool Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirementsTrkCompetingRIOsOnTrackTool Requirements
@include requirements
*/
