/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonSegmentCleaner_page MuonSegmentCleaner Package
@author Peter Kluit <s01@nikhef.nl>
@author Jochem Snuverink <jsnuverink@nikhef.nl>

@section MuonSegmentCleaner_MuonSegmentCleanerIntro Introduction

This package contains tools that clean segments, e.g. outliers, delta electrons, trigger ambiguities 

@section MuonSegmentCleaner_MuonSegmentCleanerOverview Class Overview
  The MuonSegmentCleaner package contains the following classes:

  - MuonPhiHitSelector: cleans Phi segments from outliers, build rio's, competing rio's (ambiguous hits) and perform clusterization
  - MuonSegmentAmbiCleaner: solves TGC and RPC ambiguities, extrapolating MDT segments



*/
