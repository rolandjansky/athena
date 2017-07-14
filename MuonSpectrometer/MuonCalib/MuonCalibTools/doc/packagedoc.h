/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonCalibTools_page MuonCalibTools Package
@author Niels.Van.Eldik@cern.ch, ketevi@bnl.gov, Zdenko.Van.Kesteren@cern.ch

@section MuonCalibTools_MuonCalibToolsIntro Introduction
This package contains general tools needed by the Muon Calibration Framework

@section MuonCalibTools_MuonCalibToolsOverview Class Overview

- MuonCalib::AtlasRegionSelectorTool: default Region Selector for Atlas calibration, segments are selected by chamber

- MuonCalib::TestbeamRegionSelectorTool: Region Selector for 2004 testbeam, only segments in the 2 multilayers of rotating BIL are used by the calibration

- MuonCalib::PatternNtupleMaker: creates and fills ntuple with MuonCalibEvents

- MuonCalib::IdToFixedIdTool: translates an (ATHENA) Identifier to a (Calib EDM) MuonFixedId. And vice versa.

                                                                                
                                                                                
*/
