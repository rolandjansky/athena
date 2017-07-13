/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigInDetTrackFitter_page TrigInDetTrackFitter Package

This package contains implementations of various track fitting tools for the LVL ID 
tracking. All these tools are based on the same abstract interface: ITrigInDetTrackFitter.  

@author Dmitry.Emeliyanov@cern.ch

@section TrigInDetTrackFitter_TrigInDetTrackFitIntro Introduction

The list of track fitting tools is below:

   - TrigInDetTrackFitter - a default track fitter for TrigIDSCAN
   - TrigInDetPerigeeFitter - a fast fitter which estimates only track perigee parameters and 
uses TrigSiSpacePoints directly, i.e. without dissolving them into clusters. 
   - TrigInDetKarimakiFitter - a track fitter for TrigSiTrack
   - TrigInDetSctKFitter - an old fitter based on Sijin QIAN's code

@section TrigInDetTrackFitter_TrigInDetTrackFitOverview Overview

The fitting tools make use of the following abstract interface 

   - ITrigInDetTrackFitter

This interface contains method 

void fit(TrigInDetTrackCollection* recoTracks )

where recoTracks is a collection (vector) of TrigInDetTrack tracks. A track fitter updates parameters 
of the input tracks so that no new tracks are created. 

*/  
