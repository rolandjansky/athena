/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibITools_page MdtCalibITools Package
@author Niels.Van.Eldik@cern.ch
                                                                                
@section MdtCalibITools_MdtCalibIToolsIntro Introduction
Interface classes for the various tools implemented in MdtCalibTools package.
                                                                                
@section MdtCalibITools_MdtCalibIToolsOverview Class Overview
The interface classes provided by MdtCalibITools package are:

- MuonCalib::IMdtCalibTool: Interface class for Mdt Calibration Tools 
- MuonCalib::IMdtSegmentFitterTool: Interface class for Segment Fitter Tools 
- MuonCalib::IMdtSegmentSelectorTool: Interface class for Segment Selector Tools 
- MuonCalib::IPatternSelectorTool: Interface class for Pattern Selector Tools 
                                                                                
                                                                                
Interface classes for the various tools implemented in MdtCalibTools package:

IMdtCalibTool : Interface used for Tools that are called by the MuonCalibAlg.
IPatternSelectorTool : Interface for creator tools that create IPatternSelector implemenatations.
IMdtSegmentSelectorTool : Interface for creator tools that create IMdtSegmentSelector implemenatations.
IMdtSegmentFitterTool : Interface for creator tools that create IMdtSegmentFitter implemenatations.
*/
