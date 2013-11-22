/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TrigInDetTrackUtils  Package

This package contains an implementation of TrigToTrkTrackTool class

@author D.Emeliyanov AT rlDOTacDOTuk

@section TrigInDetTrackUtilsIntro Introduction

The TrigToTrkTrackTool converts TrigInDetTrack 
objects (i.e. LVL2 tracks) into Trk::TrkTrack objects (i.e. 
offline/EF tracks). Optionally, all newly created Trk::Track
tracks can be refitted using an offline track fitter instance.

The tool has the following interface method

|code

TrigToTrkTrackTool::execute(const TrigInDetTrackCollection* lvl2Tracks,
                          TrackCollection* offlineTracks)

|endcode
    
@section TrigInDetTrackUtilsReq Requirements

@include requirements

*/
