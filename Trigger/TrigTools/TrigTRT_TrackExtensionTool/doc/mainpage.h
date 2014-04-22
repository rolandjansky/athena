/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrigTRT_TrackExtensionTool Package

This package contains implementation of the TRT track extension tool for the LVL2 ID tracking.
The tool is based on an abstract interface ITrigTRT_TrackExtensionTool

@author Dmitry.Emeliyanov@cern.ch

@section TrigTRT_TrackExtensionIntro Introduction

This tool takes a collection of TrigInDetTrack tracks and extends them into the TRT performing 
"on-the-fly" pattern recognition and track fitting. Track parameters are updated so that no new 
tracks are created. Also the tool fills the TRT-related information in TrigInDetTrack tracks 

The simultanenous pattern recognition and track fitting is accomplished by means of a Probabilistic 
Data Association Filter (PDAF). A detailed description of this method can be found in 

D. Emeliyanov, Nucl.Instr.and Meth., A566, 1, (2006) p 50.

@section TrigTRT_TrackExtensionToolOverview Overview

The main method of the tool (inherited from an abstract interface) is 

void propagate(TrigInDetTrackCollection* recoTracks )

where recoTracks is a collection (vector) of TrigInDetTrack tracks. 

*/  
