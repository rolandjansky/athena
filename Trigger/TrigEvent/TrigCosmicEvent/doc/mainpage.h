/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
@mainpage TrigCosmicEvent Package
@author Jamie.Boyd@cern.ch, Takanori.Kohno@cern.ch, Gaston.Leonardo.Romeo@cern.ch

@section TrigCosmicEventIntro Introduction

This package contains the Objects that are created by the HLT cosmic slice algorithms.

@section TrigCosmicEventOverview Class Overview
  The TrigCosmicEvent package contains of following classes:

   TrigCosmicEvent/CombinedIDHalfTrackMuonObject.h : contains difference between and average / combined track parameters from ID track and cosmic muon candidate.

   TrigCosmicEvent/CombinedIDHalfTracksObject.h : contains difference between and average / combined track parameters from 2 ID half tracks (HT) that are combined to make a long ID track (for cosmics we get 2 ID tracks separated at y=0).

   TrigCosmicEvent/CombinedIDMuonObject.h : contains difference between and average / combined track parameters from ID track and cosmic muon candidate.

   TrigCosmicEvent/CosmicMuon.h : contains cosmic muon candidate info (track params, as well as NRPC / MDT hits etc.).

   TrigCosmicEvent/MdtTrackSegment.h - contains MDT info for cosmic muon candidate and its arrival time to the different chambers 

   TrigCosmicEvent/TrigCosmicEventDict.h : ?

  

@ref used_TrigCosmicEvent

@ref requirements_TrigCosmicEvent

*/

/**
@page used_TrigCosmicEvent Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_TrigCosmicEvent Requirements
@include requirements
*/
