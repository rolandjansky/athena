/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage MuonTrackThroughCalo Package

This package contains a tool to make it easy to backtrack (forward track)
track parameters from the Muon Spectrometer to the ID or the perigee making
use of the @c Trk::Extrapolator.

@author David.Lopez@cern.ch

@section IntroductionMuonTrackThroughCalo Introduction

Several options are currently supported in the Extrapolator for transporting
candidate muon tracks through the calorimeters. MuonTrackThroughCalo provides
a tool with easy access to two extrapolators: 
  - one configured to use a parameterization of the muon energy loss, 
  - the other one configured to use a combination of the energy loss parameterization and the calorimeter measurement through Bayes theorem (see ATL-COM-MUON-2008-007).

No isolation cuts are used if the calorimeter measurement is used. 
The extrapolations supported are:
  - To the entrance of the Muon Spectrometer
  - To the perigee (closest approach to the line defined by the beam line)
  - To the interface between the ID and the Calorimeters

@section OverviewMuonTrackThroughCalo Class Overview
  The MuonTrackThroughCalo package contains the following tool:
  - Rec::MuonTrackThroughCalo : the tool to track muon parameters with either parameterised or measured E-loss
  - Rec::ITrackThroughDetectorBoundaries : the tool to track ID muon candidates while saving the parameters at all intermediate surfaces
    
@section UsageMuonTrackThroughCalo Usage 
In order to use a MuonTrackThroughCalo, a set of configurations are 
necessary. These are automatically performed for the user through
the ConfMuonTrackThroughCalo python class. This provides access to the
two type of Extrapolators described above. The expert user who may want to
change the configuration of these extrapolators is recommended to not use
MuonTrackThroughCalo, and configure the Extrapolators directly. 
MuonTrackThroughCalo is a user tool, not intended for expert use.


@section ExtrasMuonTrackThroughCalo Extra Pages

 - @ref UsedMuonTrackThroughCalo
 - @ref RequirementsMuonTrackThroughCalo
*/

/**
@page UsedMuonTrackThroughCalo Used Packages
@htmlinclude used_packages.html
*/

/**
@page RequirementsMuonTrackThroughCalo Requirements
@include requirements
*/
