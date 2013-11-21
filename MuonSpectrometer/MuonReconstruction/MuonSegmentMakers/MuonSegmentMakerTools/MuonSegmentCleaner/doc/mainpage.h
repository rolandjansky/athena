/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage MuonSegmentCleaner Package
@author Peter Kluit <s01@nikhef.nl>
@author Jochem Snuverink <jsnuverink@nikhef.nl>

@section MuonSegmentCleanerIntro Introduction

This package contains tools that clean segments, e.g. outliers, delta electrons, trigger ambiguities 

@section MuonSegmentCleanerOverview Class Overview
  The MuonSegmentCleaner package contains the following classes:

  - MuonPhiHitSelector: cleans Phi segments from outliers, build rio's, competing rio's (ambiguous hits) and perform clusterization
  - MuonSegmentAmbiCleaner: solves TGC and RPC ambiguities, extrapolating MDT segments

@ref used_MuonSegmentCleaner

@ref requirements_MuonSegmentCleaner

*/

/**
@page used_MuonSegmentCleaner Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MuonSegmentCleaner Requirements
@include requirements
*/
