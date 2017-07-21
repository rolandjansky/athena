/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibTools_page MdtCalibTools Package
@author Niels.Van.Eldik@cern.ch
                                                                                
@section MdtCalibTools_MdtCalibToolsIntro Introduction
This package contains the implementation of various tools derived from the
abstract MuonCalib::MdtCalibITools classes.
                                                                                
@section MdtCalibTools_MdtCalibToolsOverview Class Overview
The MdtCalibTools package contains the following Tools selectable from MuonCalib::MuonCalibAlg jobOptions: 
- MuonCalib::MdtCalibNtupleMakerTool: creates ntuple per calibration region containing segment information
- MuonCalib::MdtCalibTool: steers Mdt Calibration within athena

in addition it also contains the following helper Tools:
- MuonCalib::SimpleMdtSegmentSelectorTool: creates an instance of a MuonCalib::IMdtSegmentSelector implementation
- MuonCalib::SimplePatternSelectorTool: creates an instance of a MuonCalib::IGlobalPatternSelector implementation
- MuonCalib::MuCCaFitterTool: creates an instance of a MuonCalib::IMdtSegmentFitter implementation
- MuonCalib::DCSLFitterTool: creates an instance of a MuonCalib::IMdtSegmentFitter implementation
                                                                                
                                                                                
*/
