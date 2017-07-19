/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page G4AtlasTests_page G4AtlasTEsts
@author Andrea.Dellacqua@cern.ch

@section G4AtlasTests_G4AtlasTests Introduction

This package provides basic tests of the G4 Atlas simulation,
including both subdetector specific and general functionality tests.

@section G4AtlasTests_G4AtlasTests Class Overview
  The G4AtlasTests package contains of following classes:

  - AtlasTestAlg : Provides minimal tests for each subsystem and overview tests
     including truth, cpu, and memory information

  - LArHitsTestAlg : Provides tests of the LAr Calorimetery

  - PixelHitsTestAlg : Provides tests of the pixel detector

  - SctHitsTestAlg : Provides tests of the SCT detector

  - TGCHitsTestAlg : Provides tests of the TGC muon detector

  - TrtHitsTestAlg : Provides tests of the TRT detector

  - CSCHitsTestAlg : Provides tests of the CSC muon detector

  - MDTHitsTestAlg : Provides tests of the MDT muon detector

  - RPCHitsTestAlg : Provides tests of the RPC muon detector

  - TileHitsTestAlg : Provides tests of the Tile calorimeter

  - TruthHitsTestAlg : Provides tests of truth storage

  - G4TestAlg : The base class from which all classes inherit

  - TestAlgInterface : The interface to all test classes

All histograms are booked in BookHistograms.cxx - this file contains all limits for the histos.



*/
