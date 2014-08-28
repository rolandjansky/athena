/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  MuonTruthAlgs
@author Edward.Moyse@cern.ch

@section MuonTruthAlgsIntro Introduction

This package provides algorithms which produces the 'truth' information for Muon EDM classes. For example, MuonDetailedTrackTruthMaker implements the common 
tracking interface Trk::IDetailedTrackTruthBuilder.

@section MuonTruthAlgsOverview Class Overview
  The MuonTruthAlgs package contains of following classes:

   - MuonDetailedTrackTruthMaker : produces truth for tracks
   - MuonPRD_MultiTruthMaker : produces truth for MuonPrepRawData
  
@see Trk::IDetailedTrackTruthBuilder

@ref used_MuonTruthAlgs

@ref requirements_MuonTruthAlgs

*/

/**
@page used_MuonTruthAlgs Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MuonTruthAlgs Requirements
@include requirements
*/