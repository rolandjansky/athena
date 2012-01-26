/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TrigL2CosmicMuonHypo Package
@author Jamie.Boyd@cern.ch

@section TrigL2CosmicMuonHypoIntro Introduction

This package contains the Hypothesis algorithms used as part of the HLT cosmic slice. 
The algorithms in this package cut on values of objects (which live in TrigL2CosmicEvent package) which are produced by Feature Extraction Algorithms (in the HLT) which live in (TrigL2CosmicMuon and TrigL2CosmicCombined).

@section TrigL2CosmicMuonHypoOverview Class Overview
  The TrigL2CosmicMuonHypo package contains of following classes:

  - CosmicMuonHypo : cuts on the muon candidates produced by TrigL2CosmicMuon/CosmicMuon algorithm.
  - CombinedIDHalfTracksHypo : cuts on the combined ID track object produced by TrigL2CosmicCombined/CombinedIDHalfTracks algorithm.
  - CombinedIDMuonHypo : cuts on the combined ID muon object produced by TrigL2CosmicCombined/CombinedIDMuon algorithm.
  

@ref used_TrigL2CosmicMuonHypo

@ref requirements_TrigL2CosmicMuonHypo

*/

/**
@page used_TrigL2CosmicMuonHypo Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_TrigL2CosmicMuonHypo Requirements
@include requirements
*/
