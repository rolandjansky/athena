/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TrkAlignEvent
@author roberth@bu.edu
@author Daniel Kollar <daniel.kollar@cern.ch>

@section TrkAlignEventIntro Introduction

This package contains classes to be used for track-based alignment.

@section TrkAlignEventOverview Class Overview
  The package contains the following classes:

  - AlignModule: a generalized alignable module, consisting of groups of detectors to be aligned as a single structure
  - AlignModuleList: list of all AlignModules
  - AlignTSOS: inherits from TrackStateOnSurface, contains derivatives and information about AlignModule to which hit belongs
  - AlignTrack: collection of AlignTSOS and the original track from which the alignTSOS hits come from
  - AlignTrackCollection: collection of AlignTracks

@ref used_TrkAlignEvent

@ref requirements_TrkAlignEvent

*/

/**
@page used_TrkAlignEvent Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_TrkAlignEvent Requirements
@include requirements
*/
