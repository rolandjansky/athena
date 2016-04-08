/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage MdtCalibInterfaces Package
@author Niels.Van.Eldik@cern.ch
                                                                                
@section MdtCalibInterfacesIntro Introduction
This package contains all the abstract interfaces which can be used in
a standalone Muon calibration: pattern and segment selectors, calibration
procedures.
                                                                                
@section MdtCalibInterfacesOverview Class Overview
The MdtCalibInterfaces package provides the following abstract interfaces:
                                                                                
- MuonCalib::IGlobalPatternSelector: Interface to MuonGlobalPatternSelectors
- MuonCalib::IMdtCalibration: Interface to Mdt calibration implementations 
- MuonCalib::IMdtCalibrationOutput: Interface to pass calibration output during calibration
- MuonCalib::IMdtSegmentFitter: Interface for MdtSegment fitters
- MuonCalib::IMdtSegmentSelector: Interface class for MdtSegmentSelectors
- MuonCalib::IMdtCalibDBTool: interface class for Tools accessing conditions DB 

@ref used_MdtCalibInterfaces
                                                                                
@ref requirements_MdtCalibInterfaces
*/
                                                                                
/**
@page used_MdtCalibInterfaces Used Packages
@htmlinclude used_packages.html
*/
                                                                                
/**
@page requirements_MdtCalibInterfaces Requirements
@include requirements
*/

